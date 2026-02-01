#pragma once

namespace x11engine {

    class Renderer;
    class Input;

    class Application {
    public:
        virtual ~Application() = default;

        virtual bool OnCreate() = 0;
        virtual void OnUpdate(float dt) = 0;
        virtual void OnRender() = 0;
        virtual void OnResize(int width, int height) {}

        void Close() { shouldClose = true; }
        bool ShouldClose() const { return shouldClose; }

        Renderer* renderer = nullptr;
        Input* input = nullptr;

    private:
        bool shouldClose = false;
    };

} // namespace x11engine