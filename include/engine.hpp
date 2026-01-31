#pragma once

#include "app.hpp"
#include "frame.hpp"
#include "input.hpp"
#include "renderer.hpp"

#include <chrono>
#include <unistd.h>

namespace x11engine {

class Engine {
public:
    Engine(int width, int height, const std::string &title) : frame(width, height, title), renderer(width, height), running(false) {}

    bool Init() {
        if (!frame.Init())
            return false;

        if (!renderer.Init(frame))
            return false;

        if (!app.Init(&renderer, &input))
            return false;

        // Wait for window to be mapped before running logic
        WaitForMapNotify();

        running = true;
        return true;
    }

    void Run() {
        auto startTime = std::chrono::high_resolution_clock::now();
        int frameCount = 0;

        while (!app.ShouldClose()) {
            frameCount++;
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();

            if (elapsed >= 1) {
                std::string newTitle = "X11 Engine - FPS: " + std::to_string(frameCount);
                XStoreName(frame.GetDisplay(), frame.GetWindow(), newTitle.c_str());
                frameCount = 0;
                startTime = currentTime;
            }

            HandleEvents();
            Update();
            Render();

            // Cap framerate roughly ~60fps
            usleep(16000);
        }
    }

private:
    void WaitForMapNotify() {
        XEvent event;
        while (true) {
            XNextEvent(frame.GetDisplay(), &event);
            if (event.type == MapNotify)
                break;
        }
    }

    void HandleEvents() {
        XEvent event;
        while (XPending(frame.GetDisplay()) > 0) {
            XNextEvent(frame.GetDisplay(), &event);

            if (event.type == ClientMessage) {
                if ((Atom)event.xclient.data.l[0] == frame.GetWMDeleteMessage())
                    app.Close();
            }

            if (event.type == ConfigureNotify) {
                int newWidth = event.xconfigure.width;
                int newHeight = event.xconfigure.height;

                if (newWidth != renderer.GetWidth() || newHeight != renderer.GetHeight()) {
                    renderer.Resize(frame, newWidth, newHeight);
                    app.OnResize(newWidth, newHeight);
                }
            }

            input.ProcessEvent(event);
        }
    }

    void Update() {
        app.Update();
    }

    void Render() {
        app.Draw();
        renderer.Present(frame);
    }

private:
    bool running;

    Frame frame;
    Renderer renderer;
    Input input;
    App app;
};

} // namespace x11engine