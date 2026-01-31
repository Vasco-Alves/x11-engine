#include <x11engine/camera.hpp>
#include <x11engine/input.hpp> // We need the full definition of Input here
#include <x11engine/math.hpp>

#include <X11/keysym.h> // For XK_* constants
#include <cmath>

namespace x11engine::camera {

    Camera::Camera()
        : position{0.0f, 0.0f, 0.0f}, forward{0.0f, 0.0f, -1.0f}, up{0.0f, 1.0f, 0.0f}, yaw(-90.0f), pitch(0.0f), fov_degrees{74.0f},
          aspect_ratio{4.0f / 3.0f}, near_plane{0.1f}, far_plane{100.0f} {}

    void Camera::Update(const Input& input) {
        // 1. Rotation
        float rotSpeed = 2.0f;
        if (input.IsKeyDown(XK_Left))
            yaw -= rotSpeed;
        if (input.IsKeyDown(XK_Right))
            yaw += rotSpeed;
        if (input.IsKeyDown(XK_Up))
            pitch += rotSpeed;
        if (input.IsKeyDown(XK_Down))
            pitch -= rotSpeed;

        // Clamp Pitch to prevent screen flipping
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        // 2. Calculate new Forward Vector using Euler Angles
        float radYaw = math::radians(yaw);
        float radPitch = math::radians(pitch);

        math::Vec3 newForward;
        newForward.x = std::cos(radYaw) * std::cos(radPitch);
        newForward.y = std::sin(radPitch);
        newForward.z = std::sin(radYaw) * std::cos(radPitch);

        forward = math::normalize(newForward);

        // 3. Movement (FPS Strafing)
        math::Vec3 flatForward = math::normalize({forward.x, 0.0f, forward.z});
        math::Vec3 rightVector = math::normalize(math::cross(flatForward, up));

        float moveSpeed = 10.0f;

        // Move Forward/Back (W/S)
        if (input.IsKeyDown(XK_w))
            position += flatForward * moveSpeed;
        if (input.IsKeyDown(XK_s))
            position -= flatForward * moveSpeed;

        // Strafe Left/Right (A/D)
        if (input.IsKeyDown(XK_d))
            position += rightVector * moveSpeed;
        if (input.IsKeyDown(XK_a))
            position -= rightVector * moveSpeed;

        // Fly Up/Down (Q/E)
        if (input.IsKeyDown(XK_q))
            position.y += moveSpeed;
        if (input.IsKeyDown(XK_e))
            position.y -= moveSpeed;

        // Increase/Decrease FOV (Z/X)
        if (input.IsKeyDown(XK_z))
            fov_degrees -= 1.0f;
        if (input.IsKeyDown(XK_x))
            fov_degrees += 1.0f;
    }

    math::Mat4 Camera::GetViewMatrix() const noexcept { return math::lookAt(position, position + forward, up); }

    math::Mat4 Camera::GetProjectionMatrix() const noexcept {
        return math::perspective(math::radians(fov_degrees), aspect_ratio, near_plane, far_plane);
    }

} // namespace x11engine::camera