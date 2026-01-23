#pragma once

#include "input.hpp"
#include "renderer.hpp"

namespace x11engine::objects {

class Object {
public:
    Object(int x, int y) : x(x), y(y) {}
    virtual ~Object() = default;

    virtual void Update(Input &input) {}

    virtual void Draw(Renderer &renderer) = 0;

protected:
    int x, y;
};

class Square : public Object {
public:
    Square(int x, int y, int w, int h, int c) : Object(x, y), width(w), height(h), color(c) {}

    void Update(Input &input) override {
        if (input.IsKeyDown(XK_Right))
            x += 1;
        if (input.IsKeyDown(XK_Left))
            x -= 1;
        if (input.IsKeyDown(XK_Down))
            y += 1;
        if (input.IsKeyDown(XK_Up))
            y -= 1;
    }

    void Draw(Renderer &renderer) override {
        renderer.DrawRect(x, y, width, height, color);
    }

private:
    int width, height;
    int color;
};

class Cube : public Object {
public:
    Cube(int x, int y, int size, int c) : Object(x, y), size(size), color(c) {}

    void Update(Input &input) override {
        if (input.IsKeyDown(XK_Right))
            x += 1;
        if (input.IsKeyDown(XK_Left))
            x -= 1;
        if (input.IsKeyDown(XK_Down))
            y += 1;
        if (input.IsKeyDown(XK_Up))
            y -= 1;
    }

    void Draw(Renderer &renderer) override {
        // Simple representation of a cube as a square for now
        renderer.DrawRectWireframe(x, y, size, size, color);
    }

private:
    int size;
    int color;
};

} // namespace x11engine::objects
