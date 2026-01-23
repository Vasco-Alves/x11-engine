#pragma once

#include "camera.hpp"
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

        camera.SetAspectRatio(static_cast<float>(renderer->GetWidth()) / static_cast<float>(renderer->GetHeight()));

        // Objects for the scene
        objects.push_back(std::make_unique<objects::Cube>(0, 0, -200, 100, color::RED));
        objects.push_back(std::make_unique<objects::Cube>(150, 0, -200, 100, color::CYAN));
        objects.push_back(std::make_unique<objects::Cube>(-150, 0, -200, 100, color::BLUE));
        objects.push_back(std::make_unique<objects::Cube>(0, -150, -200, 100, color::MAGENTA));
        objects.push_back(std::make_unique<objects::Cube>(0, 150, -200, 100, color::GREEN));

        return true;
    }

    void Update() {
        if (input->IsKeyDown(XK_Escape))
            shouldClose = true;

        camera.Update(*input);

        for (auto &object : objects) {
            object->Update(*input);
        }
    }

    void Draw() {
        renderer->Clear(color::BLACK);

        // Compute ViewProjection Matrix
        math::Mat4 view = camera.GetViewMatrix();
        math::Mat4 proj = camera.GetProjectionMatrix();
        math::Mat4 vp = proj * view;

        for (auto &object : objects)
            object->Draw(*renderer, vp);
    }

    bool ShouldClose() { return shouldClose; }
    void Close() { shouldClose = true; }

private:
    bool shouldClose;

    Renderer *renderer;
    Input *input;
    camera::Camera camera;

    std::vector<std::unique_ptr<objects::Object>> objects;
};

} // namespace x11engine