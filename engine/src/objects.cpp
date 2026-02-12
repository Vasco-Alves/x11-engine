#include <x11engine/objects.hpp>
#include <x11engine/renderer.hpp>
#include <x11engine/input.hpp>
#include <cmath>

namespace x11engine::objects {

    using math::Vec2;
    using math::Vec4;

    // --- Object3D Implementation ---

    Object3D::Object3D(float x, float y, float z, uint32_t color) : position{x, y, z}, rotation{0.0f, 0.0f, 0.0f}, scale{1.0f, 1.0f, 1.0f}, color(color) {}

    Mat4 Object3D::GetModelMatrix() const {
        // 1. Scale
        Mat4 matScale = Mat4::scale(scale.x, scale.y, scale.z);

        // 2. Rotation
        Mat4 rotX = Mat4::rotationX(math::radians(rotation.x));
        Mat4 rotY = Mat4::rotationY(math::radians(rotation.y));
        Mat4 rotZ = Mat4::rotationZ(math::radians(rotation.z));

        Mat4 matRot = rotZ * rotY * rotX; // Z * Y * X

        // 3. Translation
        Mat4 matTrans = Mat4::translation(position.x, position.y, position.z);

        return matTrans * matRot * matScale;
    }

    void Object3D::DrawWireframe(Renderer& renderer, const Mat4& viewProj, const std::vector<std::array<int, 2>>& edges) {
        Mat4 model = GetModelMatrix();
        Mat4 mvp = viewProj * model;

        // 1. Transform ALL vertices to Clip Space
        std::vector<math::Vec4> clipSpaceVerts(vertices.size());
        for (size_t i = 0; i < vertices.size(); ++i) {
            clipSpaceVerts[i] = mvp * math::Vec4{vertices[i].x, vertices[i].y, vertices[i].z, 1.0f};
        }

        float halfW = renderer.GetWidth() * 0.5f;
        float halfH = renderer.GetHeight() * 0.5f;
        const float nearClip = 0.1f;

        // Helper: Clip Space -> Screen Space
        auto ToScreen = [&](const math::Vec4& v) -> math::Vec2 {
            float invW = 1.0f / v.w;
            return {(v.x * invW + 1.0f) * halfW, (1.0f - v.y * invW) * halfH};
        };

        // 2. Iterate over EDGES
        for (const auto& edge : edges) {
            int idx1 = edge[0];
            int idx2 = edge[1];

            // Safety check
            if (idx1 >= clipSpaceVerts.size() || idx2 >= clipSpaceVerts.size())
                continue;

            math::Vec4 v1 = clipSpaceVerts[idx1];
            math::Vec4 v2 = clipSpaceVerts[idx2];

            bool v1In = v1.w >= nearClip;
            bool v2In = v2.w >= nearClip;

            // Both behind camera
            if (!v1In && !v2In)
                continue;

            // Both visible
            if (v1In && v2In) {
                math::Vec2 p1 = ToScreen(v1);
                math::Vec2 p2 = ToScreen(v2);
                renderer.DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, color);
                continue;
            }

            // One visible, one behind (Clipping)
            if (!v1In) {
                std::swap(v1, v2); // Ensure v1 is the visible one
            }

            // v1 is IN, v2 is OUT
            float t = (nearClip - v1.w) / (v2.w - v1.w);
            math::Vec4 vClipped = v1 + (v2 - v1) * t;

            math::Vec2 p1 = ToScreen(v1);
            math::Vec2 p2 = ToScreen(vClipped);

            renderer.DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, color);
        }
    }

    // --- Cube Implementation ---

    const std::vector<std::array<int, 2>> Cube::edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Bottom face
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Top face
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Connecting pillars
    };

    Cube::Cube(float x, float y, float z, float size, uint32_t color) : Object3D(x, y, z, color) {
        scale = {size, size, size};
        // Unit Cube centered at 0,0,0
        vertices = {{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}};
    }

    void Cube::Update(const Input& input) {
        rotation.x += 1.0f;
        rotation.y += 1.0f;
        if (rotation.x >= 360.0f)
            rotation.x -= 360.0f;
        if (rotation.y >= 360.0f)
            rotation.y -= 360.0f;
    }

    void Cube::Draw(Renderer& renderer, const Mat4& viewProj) { DrawWireframe(renderer, viewProj, edges); }

    // --- TriangularPyramid Implementation ---

    const std::vector<std::array<int, 2>> TriangularPyramid::edges = {
        {0, 1}, {1, 2}, {2, 0}, // Base
        {0, 3}, {1, 3}, {2, 3}  // Sides
    };

    TriangularPyramid::TriangularPyramid(float x, float y, float z, float baseSize, float height, uint32_t color) : Object3D(x, y, z, color) {

        scale = {baseSize, height, baseSize};

        // Equilateral triangle base math
        float r = 0.5f;
        float hOffset = std::sqrt(3.0f) * r * 0.5f;

        vertices = {
            {-0.5f, -0.5f, hOffset * 0.5f}, // 0: Base Left
            {0.5f, -0.5f, hOffset * 0.5f},  // 1: Base Right
            {0.0f, -0.5f, -hOffset},        // 2: Base Top (Back)
            {0.0f, 0.5f, 0.0f}              // 3: Apex
        };
    }

    void TriangularPyramid::Update(const Input& input) {
        rotation.y -= 1.0f;
        if (rotation.y <= -360.0f)
            rotation.y += 360.0f;
    }

    void TriangularPyramid::Draw(Renderer& renderer, const Mat4& viewProj) { DrawWireframe(renderer, viewProj, edges); }

    // --- SquarePyramid Implementation ---

    const std::vector<std::array<int, 2>> SquarePyramid::edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Base
        {0, 4}, {1, 4}, {2, 4}, {3, 4}  // Sides
    };

    SquarePyramid::SquarePyramid(float x, float y, float z, float baseSize, float height, uint32_t color) : Object3D(x, y, z, color) {

        scale = {baseSize, height, baseSize};

        vertices = {
            {-0.5f, -0.5f, -0.5f}, // 0
            {0.5f, -0.5f, -0.5f},  // 1
            {0.5f, -0.5f, 0.5f},   // 2
            {-0.5f, -0.5f, 0.5f},  // 3
            {0.0f, 0.5f, 0.0f}     // 4 Apex
        };
    }

    void SquarePyramid::Update(const Input& input) {
        rotation.y += 1.0f;
        if (rotation.y >= 360.0f)
            rotation.y -= 360.0f;
    }

    void SquarePyramid::Draw(Renderer& renderer, const Mat4& viewProj) { DrawWireframe(renderer, viewProj, edges); }

    // --- Sphere Implementation ---

    Sphere::Sphere(float x, float y, float z, float radius, int rings, int sectors, uint32_t color) : Object3D(x, y, z, color) {

        scale = {radius, radius, radius}; // Scale unit sphere to radius

        // 1. Generate Vertices
        for (int r = 0; r <= rings; ++r) {
            float phi = M_PI * (float)r / (float)rings; // 0 to PI
            float sinPhi = std::sin(phi);
            float cosPhi = std::cos(phi);

            for (int s = 0; s <= sectors; ++s) {
                float theta = 2.0f * M_PI * (float)s / (float)sectors; // 0 to 2PI
                float sinTheta = std::sin(theta);
                float cosTheta = std::cos(theta);

                // Standard Spherical to Cartesian
                float vx = cosTheta * sinPhi;
                float vy = cosPhi;
                float vz = sinTheta * sinPhi;

                vertices.push_back({vx, vy, vz});
            }
        }

        // 2. Generate Edges
        for (int r = 0; r < rings; ++r) {
            for (int s = 0; s < sectors; ++s) {
                int current = r * (sectors + 1) + s;
                int next = current + 1;
                int below = current + (sectors + 1);

                sphereEdges.push_back({current, next});
                sphereEdges.push_back({current, below});
            }
        }
    }

    void Sphere::Update(const Input& input) {
        // Simple rotation animation
        rotation.x += 0.5f;
        rotation.y -= 0.5f;
        if (rotation.x > 360.0f)
            rotation.x -= 360.0f;
        if (rotation.y < -360.0f)
            rotation.y += 360.0f;
    }

    void Sphere::Draw(Renderer& renderer, const Mat4& viewProj) { DrawWireframe(renderer, viewProj, sphereEdges); }

} // namespace x11engine::objects