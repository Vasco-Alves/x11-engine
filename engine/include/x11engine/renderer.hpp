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
        Renderer(int width, int height);
        ~Renderer();

        bool Init(const Frame& frame);                                // Initialize X-specific resources (XImage)
        void Present(const Frame& frame);                             // Pushes the framebuffer to the X11 Window
        void Clear(uint32_t color);                                   // Clear the framebuffer with a specific color
        void Resize(const Frame& frame, int newWidth, int newHeight); // Resize the framebuffer

        void DrawLine(int x0, int y0, int x1, int y1, uint32_t color); // Bresenham's line algorithm
        void DrawRect(int x, int y, int w, int h, uint32_t color);
        void DrawRectWireframe(int x, int y, int w, int h, uint32_t color);

        uint32_t* GetFramebuffer() { return framebuffer; }
        int GetWidth() const { return width; }
        int GetHeight() const { return height; }

      private:
        void MapToScreenCoord(int& x, int& y); // Transform from Center-Origin to Top-Left-Origin

        void DrawPixelScreen(int x, int y, uint32_t color);
        void DrawPixel(int x, int y, uint32_t color);

      private:
        int width;
        int height;
        uint32_t* framebuffer;
        XImage* image;
    };

} // namespace x11engine