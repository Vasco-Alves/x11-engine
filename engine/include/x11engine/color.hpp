#pragma once

#include <cstdint>

namespace x11engine::color {

    // Colors are in 0xRRGGBB format.

    constexpr uint32_t RGB(uint8_t r, uint8_t g, uint8_t b) noexcept {
        return (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) | static_cast<uint32_t>(b);
    }

    constexpr uint32_t BLACK = RGB(0, 0, 0);
    constexpr uint32_t WHITE = RGB(255, 255, 255);

    constexpr uint32_t RED = RGB(255, 0, 0);
    constexpr uint32_t GREEN = RGB(0, 255, 0);
    constexpr uint32_t BLUE = RGB(0, 0, 255);

    constexpr uint32_t YELLOW = RGB(255, 255, 0);
    constexpr uint32_t CYAN = RGB(0, 255, 255);
    constexpr uint32_t MAGENTA = RGB(255, 0, 255);

    constexpr uint32_t GRAY = RGB(128, 128, 128);
    constexpr uint32_t ORANGE = RGB(255, 165, 0);
    constexpr uint32_t PURPLE = RGB(128, 0, 128);
    constexpr uint32_t BROWN = RGB(165, 42, 42);

} // namespace x11engine::color
