#pragma once

#include "input.hpp"
#include "math.hpp"
#include "renderer.hpp"

#include <vector>

namespace x11engine::objects {

using math::Mat4;
using math::Vec2;
using math::Vec3;
using math::Vec4;

class Object {
public:
    Object(int x, int y) : x(x), y(y) {}
    virtual ~Object() = default;
    virtual void Update(Input &input) = 0;
    virtual void Draw(Renderer &renderer, const Mat4 &viewProj) = 0;

protected:
    int x, y;
};

class Object3D : public Object {
public:
    Object3D(float x, float y, float z) : Object(0, 0), position{x, y, z}, rotation{0.0f, 0.0f, 0.0f}, scale{1.0f, 1.0f, 1.0f} {}

    virtual ~Object3D() = default;

    // Calculates the full Model Matrix: Translation * Rotation * Scale. Local coords -> world coords.
    Mat4 GetModelMatrix() const {
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

public:
    Vec3 position;
    Vec3 rotation;
    Vec3 scale;
};

class Cube : public Object3D {
public:
    Cube(float x, float y, float z, float size, int color) : Object3D(x, y, z), color(color) {
        scale = {size, size, size};

        // Standard unit cube (1x1x1) centered at origin
        vertices = {
            {-0.5f, -0.5f, -0.5f},
            {0.5f, -0.5f, -0.5f},
            {0.5f, 0.5f, -0.5f},
            {-0.5f, 0.5f, -0.5f},
            {-0.5f, -0.5f, 0.5f},
            {0.5f, -0.5f, 0.5f},
            {0.5f, 0.5f, 0.5f},
            {-0.5f, 0.5f, 0.5f}};
    }

    void Update(Input &input) override {
        float rotSpeed = 1.0f;

        rotation.x += rotSpeed;
        rotation.y += rotSpeed;
        rotation.z += rotSpeed;

        if (rotation.x >= 360.0f) rotation.x -= 360.0f;
        if (rotation.y >= 360.0f) rotation.y -= 360.0f;
        if (rotation.z >= 360.0f) rotation.z -= 360.0f;
    }

    void Draw(Renderer &renderer, const Mat4 &viewProj) override {
        Mat4 model = GetModelMatrix();
        Mat4 mvp = viewProj * model;

        std::vector<Vec2> projectedPoints;
        projectedPoints.reserve(vertices.size());

        // Dynamically get screen size
        float screenW = (float)renderer.GetWidth();
        float screenH = (float)renderer.GetHeight();
        float halfW = screenW * 0.5f;
        float halfH = screenH * 0.5f;

        for (const auto &v : vertices) {
            Vec4 clip = mvp * Vec4{v.x, v.y, v.z, 1.0f};

            // Simple clipping
            if (clip.w < 0.1f) {
                projectedPoints.push_back({-9999, -9999});
                continue;
            }

            Vec3 ndc = {clip.x / clip.w, clip.y / clip.w, clip.z / clip.w};

            // --- VIEWPORT MAPPING ---
            // NDC (0,0) -> Screen (HalfW, HalfH)
            float sx = (ndc.x + 1.0f) * halfW;
            float sy = (1.0f - ndc.y) * halfH; // Flip Y for X11

            projectedPoints.push_back({sx, sy});
        }

        // Draw edges (DrawLine uses raw X11 coordinates now)
        for (const auto &e : edges) {
            Vec2 p1 = projectedPoints[e[0]];
            Vec2 p2 = projectedPoints[e[1]];

            if (p1.x < -9000 || p2.x < -9000)
                continue;

            renderer.DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, color);
        }
    }

private:
    int color;
    std::vector<Vec3> vertices;
    const int edges[12][2] = {
        {0, 1},
        {1, 2},
        {2, 3},
        {3, 0},
        {4, 5},
        {5, 6},
        {6, 7},
        {7, 4},
        {0, 4},
        {1, 5},
        {2, 6},
        {3, 7}};
};

} // namespace x11engine::objects