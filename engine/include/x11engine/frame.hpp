#pragma once

#include <X11/Xlib.h>
#include <string>

namespace x11engine {

    class Frame {
      public:
        Frame(int width, int height, const std::string& title);
        ~Frame();

        bool Init();
        void Resize(int newWidth, int newHeight);

        Display* GetDisplay() const;
        Window GetWindow() const;
        int GetScreen() const;
        Atom GetWMDeleteMessage() const;

      private:
        int width;
        int height;
        std::string title;

        Display* display;
        Window window;
        int screen;
        Atom wmDeleteMessage;
    };

} // namespace x11engine