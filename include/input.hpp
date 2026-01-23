#pragma once

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unordered_map>

namespace x11engine {

class Input {
public:
    // Process X11 events to update key states
    void ProcessEvent(const XEvent &event) {
        switch (event.type) {
            case KeyPress: {
                KeySym key = XLookupKeysym(const_cast<XKeyEvent *>(&event.xkey), 0);
                keys[key] = true;
                break;
            }

            case KeyRelease: {
                KeySym key = XLookupKeysym(const_cast<XKeyEvent *>(&event.xkey), 0);
                keys[key] = false;
                break;
            }

            default:
                break;
        }
    }

    bool IsKeyDown(KeySym key) const {
        auto it = keys.find(key);
        if (it != keys.end())
            return it->second;

        return false;
    }

private:
    std::unordered_map<KeySym, bool> keys;
};

} // namespace x11engine