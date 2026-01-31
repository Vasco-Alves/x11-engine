#pragma once

#include <x11engine/math.hpp>
#include <vector>
#include <array>
#include <cstdint>

namespace x11engine {

    class Renderer; // Forward declaration
    class Input;    // Forward declaration

    namespace objects {

        using math::Mat4;
        using math::Vec3;

        class Object {
          public:
            virtual ~Object() = default;
            virtual void Update(const Input& input) = 0;
            virtual void Draw(Renderer& renderer, const Mat4& viewProj) = 0;
        };

        class Object3D : public Object {
          public:
            Object3D(float x, float y, float z);
            virtual ~Object3D() = default;

            Mat4 GetModelMatrix() const;

            Vec3 position;
            Vec3 rotation;
            Vec3 scale;
        };

        class Cube : public Object3D {
          public:
            Cube(float x, float y, float z, float size, uint32_t color);

            void Update(const Input& input) override;
            void Draw(Renderer& renderer, const Mat4& viewProj) override;

          private:
            uint32_t color;
            std::vector<Vec3> vertices;
            // We can store edges as a static member or hardcoded in cpp to save memory per instance
            static const int edges[12][2];
        };

    } // namespace objects
} // namespace x11engine