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

        // --- Base Object Interface ---
        class Object {
        public:
            virtual ~Object() = default;
            virtual void Update(const Input& input) = 0;
            virtual void Draw(Renderer& renderer, const Mat4& viewProj) = 0;
        };

        // --- Object3D Base Class ---
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

            // Updated signature to use vector reference for safety and speed
            void DrawWireframe(Renderer& renderer, const Mat4& viewProj, const std::vector<std::array<int, 2>>& edges);
        };

        // --- Cube ---
        class Cube : public Object3D {
        public:
            Cube(float x, float y, float z, float size, uint32_t color);

            void Update(const Input& input) override;
            void Draw(Renderer& renderer, const Mat4& viewProj) override;

        private:
            static const std::vector<std::array<int, 2>> edges;
        };

        // --- Triangular Pyramid ---
        class TriangularPyramid : public Object3D {
        public:
            TriangularPyramid(float x, float y, float z, float baseSize, float height, uint32_t color);

            void Update(const Input& input) override;
            void Draw(Renderer& renderer, const Mat4& viewProj) override;

        private:
            static const std::vector<std::array<int, 2>> edges;
        };

        // --- Square Pyramid ---
        class SquarePyramid : public Object3D {
        public:
            SquarePyramid(float x, float y, float z, float baseSize, float height, uint32_t color);

            void Update(const Input& input) override;
            void Draw(Renderer& renderer, const Mat4& viewProj) override;

        private:
            static const std::vector<std::array<int, 2>> edges;
        };

        // --- UV Sphere ---
        class Sphere : public Object3D {
        public:
            Sphere(float x, float y, float z, float radius, int rings, int sectors, uint32_t color);

            void Update(const Input& input) override;
            void Draw(Renderer& renderer, const Mat4& viewProj) override;

        private:
            // Sphere edges are dynamic based on rings/sectors, so they are not static
            std::vector<std::array<int, 2>> sphereEdges;
        };

    } // namespace objects
} // namespace x11engine