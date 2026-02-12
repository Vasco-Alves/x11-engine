#include <x11engine/engine.hpp>
#include <x11engine/application.hpp>
#include <x11engine/objects.hpp>
#include <x11engine/camera.hpp>
#include <x11engine/color.hpp>

#include <vector>
#include <memory>

class SandboxApp : public x11engine::Application {
public:
    bool OnCreate() override {
        // 1. Cube: Size 100 (extends -50 to +50 from center)
        objects.push_back(std::make_unique<x11engine::objects::Cube>(0.0f, 0.0f, -200.0f, 100.0f, x11engine::color::RED));

        // 2. Sphere: Radius 50 (Diameter 100). Should look same width as Cube.
        objects.push_back(std::make_unique<x11engine::objects::Sphere>(150.0f, 0.0f, -200.0f, 50.0f, 16, 32, x11engine::color::GREEN));

        // 3. Square Pyramid
        objects.push_back(std::make_unique<x11engine::objects::SquarePyramid>(-150.0f, 0.0f, -200.0f, 100.0f, 100.0f, x11engine::color::YELLOW));

        // 4. Triangular Pyramid
        objects.push_back(std::make_unique<x11engine::objects::TriangularPyramid>(0.0f, 150.0f, -200.0f, 80.0f, 100.0f, x11engine::color::MAGENTA));

        return true;
    }

    void OnUpdate(float dt) override {
        if (!input)
            return;

        if (input->IsKeyDown(XK_Escape))
            Close();

        camera.Update(*input);

        for (auto& obj : objects)
            obj->Update(*input);
    }

    void OnRender() override {
        if (!renderer)
            return;

        // Clear screen
        renderer->Clear(x11engine::color::BLACK);

        // Calculate ViewProjection Matrix
        auto view = camera.GetViewMatrix();
        auto proj = camera.GetProjectionMatrix();
        auto vp = proj * view;

        // Draw all objects
        for (auto& obj : objects)
            obj->Draw(*renderer, vp);
    }

    void OnResize(int width, int height) override {
        // Prevent division by zero
        if (height > 0)
            camera.SetAspectRatio((float)width / (float)height);
    }

private:
    x11engine::camera::Camera camera;
    std::vector<std::unique_ptr<x11engine::objects::Object>> objects;
};

int main() {
    SandboxApp game;

    x11engine::Engine engine(1280, 960, "X11 3D Engine", &game);

    if (engine.Init())
        engine.Run();

    return 0;
}