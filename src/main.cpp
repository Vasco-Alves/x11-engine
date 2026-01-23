#include "engine.hpp"

// Aspect ratio 4:3
const int WIDTH = 800;
const int HEIGHT = 600;

int main() {
    x11engine::Engine engine(WIDTH, HEIGHT, "Minimal Software Renderer (Linux)");

    if (!engine.Init())
        return 1;

    engine.Run();

    return 0;
}