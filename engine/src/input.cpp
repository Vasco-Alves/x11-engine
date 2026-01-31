#include <x11engine/input.hpp>

namespace x11engine {

    void Input::ProcessEvent(const XEvent& event) {
        if (event.type == KeyPress) {
            KeySym key = XLookupKeysym(const_cast<XKeyEvent*>(&event.xkey), 0);
            keys[key] = true;

        } else if (event.type == KeyRelease) {
            KeySym key = XLookupKeysym(const_cast<XKeyEvent*>(&event.xkey), 0);
            keys[key] = false;
        }
    }

    bool Input::IsKeyDown(KeySym key) const {
        auto it = keys.find(key);
        if (it != keys.end())
            return it->second;
        return false;
    }

} // namespace x11engine