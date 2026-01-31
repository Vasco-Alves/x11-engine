#pragma once

#include <x11engine/frame.hpp>
#include <x11engine/renderer.hpp>
#include <x11engine/input.hpp>

#include <string>
#include <memory>

namespace x11engine {

    class Application; // Forward declaration

    class Engine {
      public:
        Engine(int width, int height, const std::string& title, Application* app);
        ~Engine(); // Define in cpp to allow forward declared Application

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