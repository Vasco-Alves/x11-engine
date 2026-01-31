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
        // Create a few cubes
        objects.push_back(std::make_unique<x11engine::objects::Cube>(0, 0, -200, 100, x11engine::color::RED));
        objects.push_back(std::make_unique<x11engine::objects::Cube>(150, 50, -250, 80, x11engine::color::CYAN));
        objects.push_back(std::make_unique<x11engine::objects::Cube>(-150, -50, -250, 80, x11engine::color::GREEN));

        return true;
    }

    void OnUpdate(float dt) override {
        if (input) {
            if (input->IsKeyDown(XK_Escape))
                Close();

            camera.Update(*input);

            // Update all objects
            for (auto& obj : objects)
                obj->Update(*input);
        }
    }

    void OnRender() override {
        if (!renderer)
            return;

        renderer->Clear(x11engine::color::BLACK);

        // Calculate ViewProjection Matrix
        auto view = camera.GetViewMatrix();
        auto proj = camera.GetProjectionMatrix();
        auto vp = proj * view;

        for (auto& obj : objects)
            obj->Draw(*renderer, vp);
    }

    void OnResize(int width, int height) override {
        if (height > 0)
            camera.SetAspectRatio((float)width / (float)height);
    }

  private:
    x11engine::camera::Camera camera;
    std::vector<std::unique_ptr<x11engine::objects::Object>> objects;
};

int main() {
    SandboxApp game;

    x11engine::Engine engine(1280, 960, "X11 Sandbox", &game);

    if (engine.Init())
        engine.Run();

    return 0;
}