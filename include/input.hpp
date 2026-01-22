#pragma once

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unordered_map>

namespace x11engine {

class Input {
public:
    // Process X11 events to update key states
    void ProcessEvent(const XEvent &event) {
        if (event.type == KeyPress) {
            KeySym key = XLookupKeysym(const_cast<XKeyEvent *>(&event.xkey), 0);
            keys[key] = true;
        } else if (event.type == KeyRelease) {
            // Check for auto-repeat (X11 sends Release/Press pairs when holding a key)
            // A real engine filters this by peeking the next event,
            // but for simplicity we'll just register the release.
            KeySym key = XLookupKeysym(const_cast<XKeyEvent *>(&event.xkey), 0);
            keys[key] = false;
        }
    }

    // Check if a specific key is currently held down
    bool IsKeyDown(KeySym key) const {
        auto it = keys.find(key);
        if (it != keys.end()) {
            return it->second;
        }
        return false;
    }

private:
    std::unordered_map<KeySym, bool> keys;
};

} // namespace x11engine