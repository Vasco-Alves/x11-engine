#pragma once

#include "input.hpp"
#include "renderer.hpp"
#include "math.hpp"

namespace x11engine::objects {

class Object {
public:
    Object(int x, int y) : x(x), y(y) {}
    virtual ~Object() = default;
    virtual void Update(Input &input) = 0;
    virtual void Draw(Renderer &renderer, const math::Mat4& viewProj) = 0;

protected:
    int x, y;
};

class Square : public Object {
public:
    Square(int x, int y, int w, int h, int c) : Object(x, y), width(w), height(h), color(c) {}

    void Update(Input &input) override {
        // Coordinate system: +Y is Down (Screen coordinates)
        if (input.IsKeyDown(XK_Right)) x += 1;
        if (input.IsKeyDown(XK_Left))  x -= 1;
        if (input.IsKeyDown(XK_Down))  y += 1;
        if (input.IsKeyDown(XK_Up))    y -= 1;
    }

    void Draw(Renderer &renderer, const math::Mat4& viewProj) override {
        // --- MAPPING STEP ---
        // Map Local "Center" Coordinates -> X11 "Top-Left" Coordinates
        int screenX = renderer.GetWidth() / 2 + x;
        int screenY = renderer.GetHeight() / 2 + y;

        // DrawRect draws centered on the coordinate we give it.
        // So passing (screenX, screenY) draws the square centered at that screen pixel.
        renderer.DrawRect(screenX, screenY, width, height, color);
    }

private:
    int width, height;
    int color;
};

} // namespace x11engine::objects