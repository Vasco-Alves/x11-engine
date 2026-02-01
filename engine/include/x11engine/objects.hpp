#pragma once

#include <x11engine/math.hpp>

#include <vector>
#include <array>
#include <cstdint>

namespace x11engine {

    class Renderer;
    class Input;

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
            Object3D(float x, float y, float z, uint32_t color);
            virtual ~Object3D() = default;

            Mat4 GetModelMatrix() const;

            Vec3 position;
            Vec3 rotation;
            Vec3 scale;

        protected:
            uint32_t color;
            std::vector<Vec3> vertices;

            void DrawWireframe(Renderer& renderer, const Mat4& viewProj, const int (*edges)[2], size_t edgeCount);
        };

        // --- Cube ---

        class Cube : public Object3D {
        public:
            Cube(float x, float y, float z, float size, uint32_t color);

            void Update(const Input& input) override;
            void Draw(Renderer& renderer, const Mat4& viewProj) override;

        private:
            static const int edges[12][2];
        };

        // --- Triangular Pyramid ---

        class TriangularPyramid : public Object3D {
        public:
            TriangularPyramid(float x, float y, float z, float baseSize, float height, uint32_t color);

            void Update(const Input& input) override;
            void Draw(Renderer& renderer, const Mat4& viewProj) override;

        private:
            static const int edges[6][2]; // 3 base edges + 3 rising edges
        };

        // --- Square Pyramid ---

        class SquarePyramid : public Object3D {
        public:
            SquarePyramid(float x, float y, float z, float baseSize, float height, uint32_t color);

            void Update(const Input& input) override;
            void Draw(Renderer& renderer, const Mat4& viewProj) override;

        private:
            static const int edges[8][2]; // 4 base edges + 4 rising edges
        };

        // --- UV Sphere ---
        class Sphere : public Object3D {
        public:
            Sphere(float x, float y, float z, float radius, int rings, int sectors, uint32_t color);

            void Update(const Input& input) override;
            void Draw(Renderer& renderer, const Mat4& viewProj) override;

        private:
            std::vector<std::pair<int, int>> edges;
        };

    } // namespace objects
} // namespace x11engine