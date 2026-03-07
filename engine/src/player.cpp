#include "x11engine/player.hpp"
#include "x11engine/input.hpp"

#include <X11/keysym.h>

namespace x11engine::objects {

    const std::vector<std::array<int, 2>> Player::edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Bottom face
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Top face
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Connecting pillars
    };

    Player::Player(float x, float y, float z, float size, uint32_t color) : forward(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f), Object3D(x, y, z, color) {
        scale = {size, size, size};
        vertices = {{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}};
    }

    void Player::Update(const Input& input) {
        // rotation.x += 1.0f;
        // rotation.y += 1.0f;
        // if (rotation.x >= 360.0f)
        //     rotation.x -= 360.0f;
        // if (rotation.y >= 360.0f)
        //     rotation.y -= 360.0f;

        // Movement (FPS Strafing)
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
    }

    void Player::Draw(Renderer& renderer, const Mat4& viewProj) { DrawWireframe(renderer, viewProj, edges); }

} // namespace x11engine::objects
