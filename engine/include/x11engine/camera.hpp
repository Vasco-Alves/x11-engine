#pragma once

#include <x11engine/math.hpp>

namespace x11engine {

    class Input;

    namespace camera {

        class Camera {
        public:
            Camera();

            // Updates camera position and rotation based on user input
            void Update(const x11engine::Input& input);

            // Returns the calculated View Matrix (World -> Camera space)
            math::Mat4 GetViewMatrix() const noexcept;

            // Returns the Projection Matrix (Camera -> Clip space)
            math::Mat4 GetProjectionMatrix() const noexcept;

            void SetAspectRatio(float aspect) { aspect_ratio = aspect; }

        public:
            math::Vec3 position;
            math::Vec3 forward;
            math::Vec3 up;

            float yaw;
            float pitch;

            float fov_degrees;
            float aspect_ratio;
            float near_plane;
            float far_plane;
        };

    } // namespace camera

} // namespace x11engine
