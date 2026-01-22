#pragma once

#include "app.hpp"
#include "frame.hpp"
#include "input.hpp"
#include "renderer.hpp"

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
        while (!app.ShouldClose()) {
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

            // Handle Window Close
            if (event.type == ClientMessage) {
                if ((Atom)event.xclient.data.l[0] == frame.GetWMDeleteMessage()) {
                    app.Close();
                }
            }
            // Pass to Input Manager
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
    Frame frame;
    Renderer renderer;
    Input input;
    App app;
    bool running;
};

} // namespace x11engine