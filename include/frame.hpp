#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include <string>

namespace x11engine {

class Frame {
public:
    Frame(int width, int height, const std::string &title)
        : width(width), height(height), title(title), display(nullptr), window(0) {}

    ~Frame() {
        if (display) {
            XDestroyWindow(display, window);
            XCloseDisplay(display);
        }
    }

    bool Init() {
        display = XOpenDisplay(NULL);
        if (!display) {
            std::cerr << "Failed to open X display" << std::endl;
            return false;
        }

        screen = DefaultScreen(display);
        Window root = RootWindow(display, screen);

        window = XCreateSimpleWindow(
            display, root,
            10, 10, width, height,
            1,
            BlackPixel(display, screen),
            WhitePixel(display, screen));

        // Select events: Exposure (redraw), Key Presses/Releases, Structure (resize/close)
        XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask);

        XStoreName(display, window, title.c_str());

        // Handle the "X" close button properly
        wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, window, &wmDeleteMessage, 1);

        XMapWindow(display, window);

        return true;
    }

    Display *GetDisplay() const { return display; }
    Window GetWindow() const { return window; }
    int GetScreen() const { return screen; }
    Atom GetWMDeleteMessage() const { return wmDeleteMessage; }

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

private:
    int width;
    int height;
    std::string title;

    Display *display;
    Window window;
    int screen;
    Atom wmDeleteMessage;
};

} // namespace x11engine