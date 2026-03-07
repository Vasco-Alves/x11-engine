#pragma once

#include "x11engine/objects.hpp"

namespace x11engine::objects {

    class Player : public Object3D {
    public:
        Player(float x, float y, float z, float size, uint32_t color);

        void Update(const Input& input) override;
        void Draw(Renderer& renderer, const Mat4& viewProj) override;

    private:
        static const std::vector<std::array<int, 2>> edges;
        math::Vec3 forward;
        math::Vec3 up;
    };

} // namespace x11engine::objects
