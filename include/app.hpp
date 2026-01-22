#pragma once

#include "color.hpp"
#include "input.hpp"
#include "renderer.hpp"

#include <memory> // Required for std::unique_ptr
#include <vector>

namespace x11engine {

// 1. Abstract Base Class
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

class App {
public:
    App() : renderer(nullptr), input(nullptr), shouldClose(false) {
        objects.reserve(10);
    }

    ~App() {
        objects.clear();
    }

    bool Init(Renderer *renderer, Input *input) {
        if (!renderer || !input)
            return false;

        this->renderer = renderer;
        this->input = input;

        objects.push_back(std::make_unique<Square>(10, 10, 100, 100, color::WHITE));

        return true;
    }

    void Update() {
        if (input->IsKeyDown(XK_Escape))
            shouldClose = true;

        for (auto &object : objects) {
            object->Update(*input);
        }
    }

    void Draw() {
        renderer->clear(color::BLACK);

        for (auto &object : objects) {
            object->Draw(*renderer);
        }
    }

    bool ShouldClose() { return shouldClose; }

    void Close() {
        shouldClose = true;
    }

private:
    Renderer *renderer;
    Input *input;

    std::vector<std::unique_ptr<Object>> objects;

    bool shouldClose;
};

} // namespace x11engine