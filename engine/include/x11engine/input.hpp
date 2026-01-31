#pragma once

#include <X11/Xlib.h>
#include <unordered_map>

namespace x11engine {

    class Input {
      public:
        void ProcessEvent(const XEvent& event);
        bool IsKeyDown(KeySym key) const;

      private:
        std::unordered_map<KeySym, bool> keys;
    };

} // namespace x11engine