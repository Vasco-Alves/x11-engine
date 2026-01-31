#include <x11engine/objects.hpp>
#include <x11engine/renderer.hpp>
#include <x11engine/input.hpp>
#include <cmath>

namespace x11engine::objects {

    using math::Vec2;
    using math::Vec4;

    // --- Object3D Implementation ---

    Object3D::Object3D(float x, float y, float z) : position{x, y, z}, rotation{0.0f, 0.0f, 0.0f}, scale{1.0f, 1.0f, 1.0f} {}

    Mat4 Object3D::GetModelMatrix() const {
        // 1. Scale
        Mat4 matScale = Mat4::scale(scale.x, scale.y, scale.z);

        // 2. Rotation
        Mat4 rotX = Mat4::rotationX(math::radians(rotation.x));
        Mat4 rotY = Mat4::rotationY(math::radians(rotation.y));
        Mat4 rotZ = Mat4::rotationZ(math::radians(rotation.z));

        // Combine rotations: Z * Y * X
        Mat4 matRot = rotZ * rotY * rotX;

        // 3. Translation
        Mat4 matTrans = Mat4::translation(position.x, position.y, position.z);

        // Final Model Matrix = T * R * S
        return matTrans * matRot * matScale;
    }

    // --- Cube Implementation ---

    const int Cube::edges[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

    Cube::Cube(float x, float y, float z, float size, uint32_t color) : Object3D(x, y, z), color(color) {
        scale = {size, size, size};
        vertices = {{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f},
                    {-0.5f, -0.5f, 0.5f},  {0.5f, -0.5f, 0.5f},  {0.5f, 0.5f, 0.5f},  {-0.5f, 0.5f, 0.5f}};
    }

    void Cube::Update(const Input& input) {
        // Simple auto-rotation
        rotation.x += 1.0f;
        rotation.y += 1.0f;
        if (rotation.x >= 360.0f)
            rotation.x -= 360.0f;
        if (rotation.y >= 360.0f)
            rotation.y -= 360.0f;
    }

    // Optimized Draw (Zero Allocation)
    void Cube::Draw(Renderer& renderer, const Mat4& viewProj) {
        Mat4 model = GetModelMatrix();
        Mat4 mvp = viewProj * model;

        Vec2 projectedPoints[8];
        bool valid[8];

        float halfW = renderer.GetWidth() * 0.5f;
        float halfH = renderer.GetHeight() * 0.5f;

        // 1. Transform Vertices
        for (int i = 0; i < 8; ++i) {
            Vec4 clip = mvp * Vec4{vertices[i].x, vertices[i].y, vertices[i].z, 1.0f};

            if (clip.w < 0.1f) {
                valid[i] = false;
                continue;
            }
            valid[i] = true;

            float invW = 1.0f / clip.w;
            projectedPoints[i].x = (clip.x * invW + 1.0f) * halfW;
            projectedPoints[i].y = (1.0f - clip.y * invW) * halfH;
        }

        // 2. Draw Edges
        for (const auto& e : edges) {
            if (!valid[e[0]] || !valid[e[1]])
                continue;

            renderer.DrawLine((int)projectedPoints[e[0]].x, (int)projectedPoints[e[0]].y, (int)projectedPoints[e[1]].x, (int)projectedPoints[e[1]].y,
                              color);
        }
    }

} // namespace x11engine::objects