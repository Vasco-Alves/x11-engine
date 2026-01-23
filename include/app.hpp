#pragma once

#include "color.hpp"
#include "objects.hpp"

#include <memory>
#include <vector>

namespace x11engine {

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

        // objects.push_back(std::make_unique<objects::Square>(0, 0, 100, 100, color::WHITE));
        objects.push_back(std::make_unique<objects::Cube>(0, 0, 100, color::WHITE));

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
        renderer->Clear(color::BLACK);

        for (auto &object : objects) {
            object->Draw(*renderer);
        }
    }

    bool ShouldClose() {
        return shouldClose;
    }

    void Close() {
        shouldClose = true;
    }

private:
    Renderer *renderer;
    Input *input;

    std::vector<std::unique_ptr<objects::Object>> objects;

    bool shouldClose;
};

} // namespace x11engine