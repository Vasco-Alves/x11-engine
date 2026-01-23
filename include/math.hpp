#pragma once

#include <cassert>
#include <cmath>
#include <cstddef>
#include <utility>

namespace x11engine::math {

constexpr float pi = 3.14159265358979323846f;

constexpr float radians(float degrees) noexcept {
    return degrees * (pi / 180.0f);
}

constexpr float degrees(float radians) noexcept {
    return radians * (180.0f / pi);
}

// =====================
// Vec3
// =====================

struct Vec3 {
    float x, y, z;

    // Constructors
    constexpr Vec3() noexcept : x(0), y(0), z(0) {}
    constexpr Vec3(float x_, float y_, float z_) noexcept : x(x_), y(y_), z(z_) {}

    // Indexing
    constexpr float &operator[](std::size_t index) noexcept {
        assert(index < 3);
        switch (index) {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
        }
        std::unreachable();
    }

    constexpr const float &operator[](std::size_t index) const noexcept {
        assert(index < 3);
        switch (index) {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
        }
        std::unreachable();
    }

    // Unary
    constexpr Vec3 operator-() const noexcept {
        return {-x, -y, -z};
    }

    // Compound assignment
    constexpr Vec3 &operator+=(const Vec3 &rhs) noexcept {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    constexpr Vec3 &operator-=(const Vec3 &rhs) noexcept {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    constexpr Vec3 &operator*=(float s) noexcept {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    constexpr Vec3 &operator/=(float s) noexcept {
        assert(s != 0.0f);
        float inv = 1.0f / s;
        x *= inv;
        y *= inv;
        z *= inv;
        return *this;
    }
};

// Vec3 free operators
constexpr Vec3 operator+(Vec3 a, const Vec3 &b) noexcept { return a += b; }
constexpr Vec3 operator-(Vec3 a, const Vec3 &b) noexcept { return a -= b; }
constexpr Vec3 operator*(Vec3 v, float s) noexcept { return v *= s; }
constexpr Vec3 operator*(float s, Vec3 v) noexcept { return v *= s; }
constexpr Vec3 operator/(Vec3 v, float s) noexcept { return v /= s; }

// Vec3 math
constexpr float dot(const Vec3 &a, const Vec3 &b) noexcept {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

constexpr Vec3 cross(const Vec3 &a, const Vec3 &b) noexcept {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

inline float length(const Vec3 &v) noexcept {
    return std::sqrt(dot(v, v));
}

inline Vec3 normalize(const Vec3 &v) noexcept {
    float len = length(v);
    assert(len > 0.0f);
    return v / len;
}

// =====================
// Vec4
// =====================

struct Vec4 {
    float x, y, z, w;

    constexpr Vec4() noexcept : x(0), y(0), z(0), w(0) {}
    constexpr Vec4(float x_, float y_, float z_, float w_) noexcept
        : x(x_), y(y_), z(z_), w(w_) {}

    constexpr float &operator[](std::size_t index) noexcept {
        assert(index < 4);
        switch (index) {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            case 3:
                return w;
        }
        std::unreachable();
    }

    constexpr const float &operator[](std::size_t index) const noexcept {
        assert(index < 4);
        switch (index) {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            case 3:
                return w;
        }
        std::unreachable();
    }
};

// Vec4 arithmetic
constexpr Vec4 operator+(const Vec4 &a, const Vec4 &b) noexcept {
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

constexpr Vec4 operator-(const Vec4 &a, const Vec4 &b) noexcept {
    return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

constexpr Vec4 operator*(const Vec4 &v, float s) noexcept {
    return {v.x * s, v.y * s, v.z * s, v.w * s};
}

constexpr Vec4 operator*(float s, const Vec4 &v) noexcept {
    return v * s;
}

// =====================
// Mat4 (column-major)
// =====================

struct Mat4 {
    Vec4 c0, c1, c2, c3;

    constexpr Mat4() noexcept : c0{}, c1{}, c2{}, c3{} {}

    constexpr Mat4(const Vec4 &a, const Vec4 &b, const Vec4 &c, const Vec4 &d) noexcept
        : c0(a), c1(b), c2(c), c3(d) {}

    constexpr Vec4 &operator[](std::size_t index) noexcept {
        assert(index < 4);
        switch (index) {
            case 0:
                return c0;
            case 1:
                return c1;
            case 2:
                return c2;
            case 3:
                return c3;
        }
        std::unreachable();
    }

    constexpr const Vec4 &operator[](std::size_t index) const noexcept {
        assert(index < 4);
        switch (index) {
            case 0:
                return c0;
            case 1:
                return c1;
            case 2:
                return c2;
            case 3:
                return c3;
        }
        std::unreachable();
    }

    // Static constructors
    static constexpr Mat4 identity() noexcept {
        return {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}};
    }

    static constexpr Mat4 translation(float x, float y, float z) noexcept {
        return {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {x, y, z, 1}};
    }

    static constexpr Mat4 scale(float x, float y, float z) noexcept {
        return {
            {x, 0, 0, 0},
            {0, y, 0, 0},
            {0, 0, z, 0},
            {0, 0, 0, 1}};
    }
};

// Matrix multiplication
constexpr Vec4 operator*(const Mat4 &m, const Vec4 &v) noexcept {
    return {
        m.c0.x * v.x + m.c1.x * v.y + m.c2.x * v.z + m.c3.x * v.w,
        m.c0.y * v.x + m.c1.y * v.y + m.c2.y * v.z + m.c3.y * v.w,
        m.c0.z * v.x + m.c1.z * v.y + m.c2.z * v.z + m.c3.z * v.w,
        m.c0.w * v.x + m.c1.w * v.y + m.c2.w * v.z + m.c3.w * v.w};
}

constexpr Mat4 operator*(const Mat4 &a, const Mat4 &b) noexcept {
    return {
        a * b.c0,
        a * b.c1,
        a * b.c2,
        a * b.c3};
}

// =====================
// Transform helpers
// =====================

inline Vec3 transformPoint(const Mat4 &m, const Vec3 &v) {
    Vec4 r = m * Vec4{v.x, v.y, v.z, 1.0f};
    return {r.x, r.y, r.z};
}

inline Vec3 transformVector(const Mat4 &m, const Vec3 &v) {
    Vec4 r = m * Vec4{v.x, v.y, v.z, 0.0f};
    return {r.x, r.y, r.z};
}

// =====================
// Projection & View
// =====================

inline Mat4 perspective(float fovY, float aspect, float near, float far) {
    float f = 1.0f / std::tan(fovY * 0.5f);

    return {
        {f / aspect, 0, 0, 0},
        {0, f, 0, 0},
        {0, 0, (far + near) / (near - far), -1},
        {0, 0, (2 * far * near) / (near - far), 0}};
}

inline Mat4 lookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up) {
    Vec3 f = normalize(center - eye);
    Vec3 s = normalize(cross(f, up));
    Vec3 u = cross(s, f);

    return {
        {s.x, u.x, -f.x, 0},
        {s.y, u.y, -f.y, 0},
        {s.z, u.z, -f.z, 0},
        {-dot(s, eye), -dot(u, eye), dot(f, eye), 1}};
}

} // namespace x11engine::math
