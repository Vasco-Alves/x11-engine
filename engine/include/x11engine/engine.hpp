#pragma once

#include <x11engine/frame.hpp>
#include <x11engine/renderer.hpp>
#include <x11engine/input.hpp>

#include <string>
#include <memory>

namespace x11engine {

    const double TICK_RATE = 60.0;
    const double TARGET_FPS = 0.0;

    class Application;

    class Engine {
    public:
        Engine(int width, int height, const std::string& title, Application* app);
        ~Engine();

        bool Init();
        void Run();

    private:
        void WaitForMapNotify();
        void HandleEvents();

        Frame frame;
        Renderer renderer;
        Input input;
        Application* app;
        bool running;
    };

} // namespace x11engine