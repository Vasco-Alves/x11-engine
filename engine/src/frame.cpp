#include <X11/Xutil.h>
#include <x11engine/frame.hpp>
#include <iostream>

namespace x11engine {

    Frame::Frame(int width, int height, const std::string& title) : width(width), height(height), title(title), display(nullptr), window(0) {}

    Frame::~Frame() {
        if (display) {
            XDestroyWindow(display, window);
            XCloseDisplay(display);
        }
    }

    bool Frame::Init() {
        display = XOpenDisplay(NULL);
        if (!display) {
            std::cerr << "Failed to open X display" << std::endl;
            return false;
        }

        screen = DefaultScreen(display);
        Window root = RootWindow(display, screen);

        window = XCreateSimpleWindow(display, root, 10, 10, width, height, 1, BlackPixel(display, screen), WhitePixel(display, screen));

        XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask);
        XStoreName(display, window, title.c_str());

        wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, window, &wmDeleteMessage, 1);

        XMapWindow(display, window);

        return true;
    }

    void Frame::Resize(int newWidth, int newHeight) {
        width = newWidth;
        height = newHeight;
        XResizeWindow(display, window, width, height);
    }

    Display* Frame::GetDisplay() const { return display; }
    Window Frame::GetWindow() const { return window; }
    int Frame::GetScreen() const { return screen; }
    Atom Frame::GetWMDeleteMessage() const { return wmDeleteMessage; }

} // namespace x11engine