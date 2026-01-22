#pragma once

#include "color.hpp"
#include "frame.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cstdint>
#include <cstring>

namespace x11engine {

class Renderer {
public:
    Renderer(int width, int height)
        : width(width), height(height), framebuffer(nullptr), image(nullptr) {

        framebuffer = new uint32_t[width * height];
        clear(color::BLACK);
    }

    ~Renderer() {
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

    // Initialize X-specific resources (XImage)
    bool Init(const Frame &frame) {
        Visual *visual = DefaultVisual(frame.GetDisplay(), frame.GetScreen());
        int depth = DefaultDepth(frame.GetDisplay(), frame.GetScreen());

        image = XCreateImage(
            frame.GetDisplay(), visual, depth,
            ZPixmap, 0,
            reinterpret_cast<char *>(framebuffer),
            width, height, 32, 0);

        return image != nullptr;
    }

    // Pushes the framebuffer to the X11 Window
    void Present(const Frame &frame) {
        if (!image)
            return;

        XPutImage(
            frame.GetDisplay(), frame.GetWindow(),
            DefaultGC(frame.GetDisplay(), frame.GetScreen()),
            image,
            0, 0, 0, 0,
            width, height);

        // Sync to ensure commands are processed
        XSync(frame.GetDisplay(), False);
    }

    void DrawPixel(int x, int y, int color) {
        if (x >= 0 && x < width && y >= 0 && y < height)
            framebuffer[y * width + x] = color;
    }

    void DrawRect(int x, int y, int w, int h, int color) {
        int startX = std::max(0, x);
        int startY = std::max(0, y);
        int endX = std::min(width, x + w);
        int endY = std::min(height, y + h);

        for (int py = startY; py < endY; ++py)
            for (int px = startX; px < endX; ++px)
                framebuffer[py * width + px] = color;
    }

    void DrawLine(int srcX, int srcY, int destX, int destY, int color) {
    }

    void clear(int color) {
        std::memset(framebuffer, color, (width * height) * sizeof(uint32_t));
    }

    uint32_t *GetFramebuffer() { return framebuffer; }

private:
    int width;
    int height;
    uint32_t *framebuffer;
    XImage *image;
};

} // namespace x11engine