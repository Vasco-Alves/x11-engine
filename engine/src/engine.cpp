#include <x11engine/engine.hpp>
#include <x11engine/application.hpp>

#include <chrono>
#include <iostream>
#include <unistd.h>

namespace x11engine {

    Engine::Engine(int width, int height, const std::string& title, Application* app)
        : frame(width, height, title), renderer(width, height), app(app), running(true) {}

    Engine::~Engine() {
        // Cleanup if needed
    }

    bool Engine::Init() {
        if (!frame.Init())
            return false;

        if (!renderer.Init(frame))
            return false;

        // Inject subsystems into the app
        if (app) {
            app->renderer = &renderer;
            app->input = &input;
            if (!app->OnCreate())
                return false;
        }

        WaitForMapNotify();
        return true;
    }

    void Engine::WaitForMapNotify() {
        XEvent event;
        while (true) {
            XNextEvent(frame.GetDisplay(), &event);
            if (event.type == MapNotify)
                break;
        }
    }

    void Engine::HandleEvents() {
        XEvent event;
        while (XPending(frame.GetDisplay()) > 0) {
            XNextEvent(frame.GetDisplay(), &event);

            if (event.type == ClientMessage) {
                if ((Atom)event.xclient.data.l[0] == frame.GetWMDeleteMessage())
                    running = false;
            }

            if (event.type == ConfigureNotify) {
                int newW = event.xconfigure.width;
                int newH = event.xconfigure.height;
                if (newW != renderer.GetWidth() || newH != renderer.GetHeight()) {
                    renderer.Resize(frame, newW, newH);
                    if (app)
                        app->OnResize(newW, newH);
                }
            }

            input.ProcessEvent(event);
        }
    }

    void Engine::Run() {
        using namespace std::chrono;

        auto startTime = high_resolution_clock::now();
        auto lastTime = high_resolution_clock::now();

        // --- Configuration ---
        const double tickRate = TICK_RATE; // Logic updates per second
        const double dt = 1.0 / tickRate;  // Constant time step

        double targetFps = TARGET_FPS;         // Desired rendering FPS
        double minFrameTime = 1.0 / targetFps; // Minimum duration of one frame

        double accumulator = 0.0;
        int frameCount = 0;

        while (running) {
            if (app && app->ShouldClose())
                running = false;

            auto currentTime = high_resolution_clock::now();
            double frameTime = duration<double>(currentTime - lastTime).count();
            lastTime = currentTime;

            if (frameTime > 0.25)
                frameTime = 0.25;

            accumulator += frameTime;

            // 1. Process Events (Input)
            HandleEvents();

            // 2. Fixed Update Loop
            while (accumulator >= dt) {
                // In the new structure, we delegate Update to the App!
                if (app)
                    app->OnUpdate(dt);
                accumulator -= dt;
            }

            // 3. Render
            if (app)
                app->OnRender();
            renderer.Present(frame);

            // 4. Performance Monitoring
            frameCount++;
            auto elapsedTotal = duration_cast<seconds>(currentTime - startTime).count();
            if (elapsedTotal >= 1) {
                std::string newTitle = "X11 Engine - FPS: " + std::to_string(frameCount) + " | TPS: " + std::to_string((int)tickRate);
                XStoreName(frame.GetDisplay(), frame.GetWindow(), newTitle.c_str());
                frameCount = 0;
                startTime = currentTime;
            }

            // 5. FPS Capping
            auto frameEndTime = high_resolution_clock::now();
            double actualFrameDuration = duration<double>(frameEndTime - currentTime).count();

            if (actualFrameDuration < minFrameTime)
                usleep(static_cast<useconds_t>((minFrameTime - actualFrameDuration) * 1000000));
        }
    }

} // namespace x11engine
