#pragma once

#include "math.hpp"

namespace x11engine::camera {

class Camera {
public:
    Camera()
        : position{0.0f, 0.0f, 0.0f},
          forward{0.0f, 0.0f, -1.0f},
          up{0.0f, 1.0f, 0.0f},
          fov_degrees{90.0f},
          aspect_ratio{4.0f / 3.0f},
          near_plane{0.1f},
          far_plane{100.0f} {}

    math::Mat4 GetViewMatrix() const noexcept {
        math::Vec3 f = math::normalize(forward);
        math::Vec3 u = math::normalize(up);
        return math::lookAt(position, position + f, u);
    }

    math::Mat4 GetProjectionMatrix() const noexcept {
        return math::perspective(
            math::radians(fov_degrees),
            aspect_ratio,
            near_plane,
            far_plane);
    }

public:
    math::Vec3 position;
    math::Vec3 forward;
    math::Vec3 up;
    float fov_degrees;
    float aspect_ratio;
    float near_plane;
    float far_plane;
};

} // namespace x11engine::camera
