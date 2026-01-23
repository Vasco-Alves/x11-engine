#pragma once

#include "color.hpp"
#include "frame.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <algorithm>
#include <cstdint>
#include <cstring>

namespace x11engine {

class Renderer {
public:
    Renderer(int width, int height)
        : width(width), height(height), framebuffer(nullptr), image(nullptr) {

        framebuffer = new uint32_t[width * height];
        Clear(color::BLACK);
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

    // Bresenham's line algorithm
    void DrawLine(int x0, int y0, int x1, int y1, uint32_t color) {
        MapToScreenCoord(x0, y0);
        MapToScreenCoord(x1, y1);

        int dx = std::abs(x1 - x0);
        int dy = -std::abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1;
        int sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;

        while (true) {
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

    void DrawRect(int x, int y, int w, int h, uint32_t color) {
        MapToScreenCoord(x, y);

        int startX = std::max(0, x - w / 2);
        int startY = std::max(0, y - h / 2);
        int endX = std::min(width, x + w / 2);
        int endY = std::min(height, y + h / 2);

        // 3. Draw only the valid visible part
        for (int py = startY; py < endY; ++py) {
            // Optimization: Calculate row offset once per row
            int rowOffset = py * width;
            for (int px = startX; px < endX; ++px) {
                framebuffer[rowOffset + px] = color;
            }
        }
    }

    void DrawRectWireframe(int x, int y, int w, int h, uint32_t color) {
        MapToScreenCoord(x, y);

        int left = x - w / 2;
        int right = x + w / 2;
        int top = y - h / 2;
        int bottom = y + h / 2;

        // Top edge
        for (int px = left; px <= right; ++px)
            DrawPixelScreen(px, top, color);

        // Bottom edge
        for (int px = left; px <= right; ++px)
            DrawPixelScreen(px, bottom, color);

        // Left edge
        for (int py = top; py <= bottom; ++py)
            DrawPixelScreen(left, py, color);
        // Right edge
        for (int py = top; py <= bottom; ++py)
            DrawPixelScreen(right, py, color);
    }

    void Clear(uint32_t color) {
        std::fill_n(framebuffer, width * height, color);
    }

    uint32_t *GetFramebuffer() { return framebuffer; }

private:
    // Transform from Center-Origin to Top-Left-Origin
    void MapToScreenCoord(int &x, int &y) {
        x += width / 2;
        y += height / 2;
    }

    void DrawPixelScreen(int x, int y, uint32_t color) {
        if (x >= 0 && x < width && y >= 0 && y < height)
            framebuffer[y * width + x] = color;
    }

    void DrawPixel(int x, int y, uint32_t color) {
        MapToScreenCoord(x, y);
        DrawPixelScreen(x, y, color);
    }

private:
    int width;
    int height;
    uint32_t *framebuffer;
    XImage *image;
};

} // namespace x11engine