#include <x11engine/renderer.hpp>

namespace {
    const int INSIDE = 0; // 0000
    const int LEFT = 1;   // 0001
    const int RIGHT = 2;  // 0010
    const int BOTTOM = 4; // 0100
    const int TOP = 8;    // 1000

    // Helper to calculate region code for a point(x, y)
    int ComputeOutCode(int x, int y, int w, int h) {
        int code = INSIDE;
        if (x < 0)
            code |= LEFT;
        else if (x >= w)
            code |= RIGHT;
        if (y < 0)
            code |= BOTTOM;
        else if (y >= h)
            code |= TOP;
        return code;
    }
} // namespace

namespace x11engine {

    Renderer::Renderer(int width, int height) : width(width), height(height), framebuffer(nullptr), image(nullptr) {
        framebuffer = new uint32_t[width * height];
        Clear(color::BLACK);
    }

    Renderer::~Renderer() {
        if (image) {
            // XDestroyImage frees the data buffer if we don't handle it carefully.
            // Since we allocated 'framebuffer' ourselves, we usually want to free it ourselves
            // or let XDestroyImage do it.
            // Standard trick: set data to NULL before destroying if you want to keep ownership,
            // OR let XImage take ownership. Here we manually delete framebuffer, so we decouple it.
            image->data = NULL;
            XDestroyImage(image);
        }
        delete[] framebuffer;
    }

    bool Renderer::Init(const Frame& frame) {
        Visual* visual = DefaultVisual(frame.GetDisplay(), frame.GetScreen());
        int depth = DefaultDepth(frame.GetDisplay(), frame.GetScreen());

        image = XCreateImage(frame.GetDisplay(), visual, depth, ZPixmap, 0, reinterpret_cast<char*>(framebuffer), width, height, 32, 0);

        return image != nullptr;
    }

    void Renderer::Present(const Frame& frame) {
        if (!image)
            return;

        XPutImage(frame.GetDisplay(), frame.GetWindow(), DefaultGC(frame.GetDisplay(), frame.GetScreen()), image, 0, 0, 0, 0, width, height);

        // Sync to ensure commands are processed
        XSync(frame.GetDisplay(), False);
    }

    void Renderer::Clear(uint32_t color) {
        // std::fill_n(framebuffer, width * height, color);
        memset(framebuffer, 0x001100, width * height * sizeof(uint32_t));
    }

    void Renderer::Resize(const Frame& frame, int newWidth, int newHeight) {
        if (newWidth == width && newHeight == height)
            return;

        // 1. Clean up old resources
        if (image) {
            image->data = NULL; // Decouple before destroying
            XDestroyImage(image);
        }
        delete[] framebuffer;

        // 2. Update dimensions and allocate new buffer
        width = newWidth;
        height = newHeight;
        framebuffer = new uint32_t[width * height];

        // 3. Re-initialize XImage
        Init(frame);
    }

    void Renderer::MapToScreenCoord(int& x, int& y) {
        x += width / 2;
        y += height / 2;
    }

    void Renderer::DrawPixelScreen(int x, int y, uint32_t color) {
        if (x >= 0 && x < width && y >= 0 && y < height)
            framebuffer[y * width + x] = color;
    }

    void Renderer::DrawPixel(int x, int y, uint32_t color) {
        MapToScreenCoord(x, y);
        DrawPixelScreen(x, y, color);
    }

    void Renderer::DrawLine(int x0, int y0, int x1, int y1, uint32_t color) {
        // --- 1. Cohen-Sutherland 2D Clipping ---
        int outcode0 = ComputeOutCode(x0, y0, width, height);
        int outcode1 = ComputeOutCode(x1, y1, width, height);
        bool accept = false;

        while (true) {
            if (!(outcode0 | outcode1)) {
                // Both points inside - trivial accept
                accept = true;
                break;
            } else if (outcode0 & outcode1) {
                // Both points share an outside zone - trivial reject
                break;
            } else {
                // One inside, one outside: Clip
                int x, y;
                // Pick the outside point
                int outcodeOut = outcode0 ? outcode0 : outcode1;

                // Find intersection point
                // Use floating point for precision during clip, then cast back
                if (outcodeOut & TOP) { // Above clip window
                    x = x0 + (x1 - x0) * (height - 1 - y0) / (double)(y1 - y0);
                    y = height - 1;
                } else if (outcodeOut & BOTTOM) { // Below clip window
                    x = x0 + (x1 - x0) * (0 - y0) / (double)(y1 - y0);
                    y = 0;
                } else if (outcodeOut & RIGHT) { // Right of clip window
                    y = y0 + (y1 - y0) * (width - 1 - x0) / (double)(x1 - x0);
                    x = width - 1;
                } else if (outcodeOut & LEFT) { // Left of clip window
                    y = y0 + (y1 - y0) * (0 - x0) / (double)(x1 - x0);
                    x = 0;
                }

                // Update the point we moved
                if (outcodeOut == outcode0) {
                    x0 = x;
                    y0 = y;
                    outcode0 = ComputeOutCode(x0, y0, width, height);
                } else {
                    x1 = x;
                    y1 = y;
                    outcode1 = ComputeOutCode(x1, y1, width, height);
                }
            }
        }

        if (!accept)
            return;

        // --- 2. Original Bresenham's Algorithm (Unchanged) ---
        // Now x0,y0 and x1,y1 are guaranteed to be on-screen

        int dx = std::abs(x1 - x0);
        int dy = -std::abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1;
        int sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;

        while (true) {
            // We can use the unsafe fast draw here because we clipped bounds!
            // But stick to DrawPixelScreen for safety if you prefer.
            // framebuffer[y0 * width + x0] = color;
            DrawPixelScreen(x0, y0, color);

            if (x0 == x1 && y0 == y1)
                break;
            int e2 = 2 * err;
            if (e2 >= dy) {
                err += dy;
                x0 += sx;
            }
            if (e2 <= dx) {
                err += dx;
                y0 += sy;
            }
        }
    }

} // namespace  x11engine
