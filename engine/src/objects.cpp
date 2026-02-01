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

        // Combine rotations: Z * Y * X
        Mat4 matRot = rotZ * rotY * rotX;

        // 3. Translation
        Mat4 matTrans = Mat4::translation(position.x, position.y, position.z);

        // Final Model Matrix = T * R * S
        return matTrans * matRot * matScale;
    }

    void Object3D::DrawWireframe(Renderer& renderer, const Mat4& viewProj, const int (*edges)[2], size_t edgeCount) {
        Mat4 model = GetModelMatrix();
        Mat4 mvp = viewProj * model;

        // 1. Transform ALL vertices to Clip Space (Vec4)
        // We keep them as Vec4 so we can inspect 'w' before dividing
        std::vector<math::Vec4> clipSpaceVerts(vertices.size());
        for (size_t i = 0; i < vertices.size(); ++i) {
            clipSpaceVerts[i] = mvp * math::Vec4{vertices[i].x, vertices[i].y, vertices[i].z, 1.0f};
        }

        float halfW = renderer.GetWidth() * 0.5f;
        float halfH = renderer.GetHeight() * 0.5f;

        // Defines how close things can be before we cut them (The Near Plane)
        const float nearClip = 0.1f;

        // Helper: Convert Clip Space (Vec4) to Screen Space (Vec2)
        auto ToScreen = [&](const math::Vec4& v) -> math::Vec2 {
            float invW = 1.0f / v.w;
            return {(v.x * invW + 1.0f) * halfW, (1.0f - v.y * invW) * halfH};
        };

        // 2. Iterate over EDGES to handle visibility
        for (size_t i = 0; i < edgeCount; ++i) {
            math::Vec4 v1 = clipSpaceVerts[edges[i][0]];
            math::Vec4 v2 = clipSpaceVerts[edges[i][1]];

            // A point is "visible" if it is in front of the Near Plane (w >= 0.1)
            bool v1In = v1.w >= nearClip;
            bool v2In = v2.w >= nearClip;

            // CASE A: Both points are behind the camera -> Skip completely
            if (!v1In && !v2In)
                continue;

            // CASE B: Both points are visible -> Draw normally
            if (v1In && v2In) {
                math::Vec2 p1 = ToScreen(v1);
                math::Vec2 p2 = ToScreen(v2);
                renderer.DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, color);
                continue;
            }

            // CASE C: One visible, one behind camera -> CLIP!
            // We need to find the point 't' where the line hits the Near Plane

            // Ensure v1 is the visible one (swap if needed) for simpler math
            if (!v1In) {
                math::Vec4 temp = v1;
                v1 = v2;
                v2 = temp;
            }
            // Now: v1 is IN (visible), v2 is OUT (behind camera)

            // Interpolation formula:
            // We want to find t such that: interpolate(v1.w, v2.w, t) == nearClip
            // v1.w + t * (v2.w - v1.w) = nearClip
            float t = (nearClip - v1.w) / (v2.w - v1.w);

            // Calculate the new clipped vertex exactly on the near plane
            // Your math library supports Vec4 operators, making this easy:
            math::Vec4 vClipped = v1 + (v2 - v1) * t;

            // Draw from the visible vertex (v1) to the new clipped vertex
            math::Vec2 p1 = ToScreen(v1);
            math::Vec2 p2 = ToScreen(vClipped);

            renderer.DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, color);
        }
    }

    // void Object3D::DrawWireframe(Renderer& renderer, const Mat4& viewProj, const int (*edges)[2], size_t edgeCount) {
    //     Mat4 model = GetModelMatrix();
    //     Mat4 mvp = viewProj * model;

    //     // Use a dynamic vector or a fixed max buffer if you know vertex count limit
    //     // Since vertices are member variables, we know vertices.size()
    //     std::vector<math::Vec2> projectedPoints(vertices.size());
    //     std::vector<bool> valid(vertices.size());

    //     float halfW = renderer.GetWidth() * 0.5f;
    //     float halfH = renderer.GetHeight() * 0.5f;

    //     // 1. Project All Vertices
    //     for (size_t i = 0; i < vertices.size(); ++i) {
    //         math::Vec4 clip = mvp * math::Vec4{vertices[i].x, vertices[i].y, vertices[i].z, 1.0f};

    //         // Simple clipping check (behind camera)
    //         if (clip.w < 0.1f) {
    //             valid[i] = false;
    //             continue;
    //         }
    //         valid[i] = true;

    //         // Perspective Divide & Viewport Transform
    //         float invW = 1.0f / clip.w;
    //         projectedPoints[i].x = (clip.x * invW + 1.0f) * halfW;
    //         projectedPoints[i].y = (1.0f - clip.y * invW) * halfH;
    //     }

    //     // 2. Draw Edges using the generic list passed in
    //     for (size_t i = 0; i < edgeCount; ++i) {
    //         int idx1 = edges[i][0];
    //         int idx2 = edges[i][1];

    //         if (valid[idx1] && valid[idx2]) {
    //             renderer.DrawLine((int)projectedPoints[idx1].x, (int)projectedPoints[idx1].y, (int)projectedPoints[idx2].x, (int)projectedPoints[idx2].y, color);
    //         }
    //     }
    // }

    // --- Cube Implementation ---

    const int Cube::edges[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

    Cube::Cube(float x, float y, float z, float size, uint32_t color) : Object3D(x, y, z, color) {
        scale = {size, size, size};
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

    // Optimized Draw (Zero Allocation)
    void Cube::Draw(Renderer& renderer, const Mat4& viewProj) { DrawWireframe(renderer, viewProj, edges, 12); }

    // --- TriangularPyramid Implementation ---

    // 0-2: Base, 3: Apex
    const int TriangularPyramid::edges[6][2] = {
        {0, 1}, {1, 2}, {2, 0}, // Base
        {0, 3}, {1, 3}, {2, 3}  // Sides
    };

    TriangularPyramid::TriangularPyramid(float x, float y, float z, float baseSize, float height, uint32_t color) : Object3D(x, y, z, color) {
        scale = {baseSize, height, baseSize};

        // Calculate base triangle offsets (equilateral triangle math)
        float r = 0.5f;                             // Radius from center to corner
        float hOffset = std::sqrt(3.0f) * r * 0.5f; // To center the triangle

        vertices = {
            // Base Triangle (y = -0.5)
            {-0.5f, -0.5f, hOffset * 0.5f}, // 0: Bottom-Left
            {0.5f, -0.5f, hOffset * 0.5f},  // 1: Bottom-Right
            {0.0f, -0.5f, -hOffset},        // 2: Top (back)

            // Apex (y = 0.5)
            {0.0f, 0.5f, 0.0f} // 3: Apex
        };
    }

    void TriangularPyramid::Update(const Input& input) {
        rotation.x += 0.5f;
        rotation.y -= 1.0f;

        if (rotation.x <= 360.0f)
            rotation.x -= 360.0f;

        if (rotation.y <= -360.0f)
            rotation.y += 360.0f;
    }

    void TriangularPyramid::Draw(Renderer& renderer, const Mat4& viewProj) { DrawWireframe(renderer, viewProj, edges, 6); }

    // --- SquarePyramid Implementation ---

    // 0-3: Base, 4: Apex
    const int SquarePyramid::edges[8][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Base
        {0, 4}, {1, 4}, {2, 4}, {3, 4}  // Sides
    };

    SquarePyramid::SquarePyramid(float x, float y, float z, float baseSize, float height, uint32_t color) : Object3D(x, y, z, color) {
        scale = {baseSize, height, baseSize};

        // Define unit vertices (Base at y=-0.5, Apex at y=0.5)
        vertices = {
            {-0.5f, -0.5f, -0.5f}, // 0: Base Back-Left
            {0.5f, -0.5f, -0.5f},  // 1: Base Back-Right
            {0.5f, -0.5f, 0.5f},   // 2: Base Front-Right
            {-0.5f, -0.5f, 0.5f},  // 3: Base Front-Left
            {0.0f, 0.5f, 0.0f}     // 4: Apex
        };
    }

    void SquarePyramid::Update(const Input& input) {
        rotation.y += 1.0f;

        if (rotation.y >= 360.0f)
            rotation.y -= 360.0f;
    }

    void SquarePyramid::Draw(Renderer& renderer, const Mat4& viewProj) { DrawWireframe(renderer, viewProj, edges, 8); }

    // --- UV Sphere Implementation ---

    Sphere::Sphere(float x, float y, float z, float radius, int rings, int sectors, uint32_t color) : Object3D(x, y, z, color) {}

    void Sphere::Update(const Input& input) {}

    void Sphere::Draw(Renderer& renderer, const Mat4& viewProj) {}

} // namespace x11engine::objects