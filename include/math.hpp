
#pragma once

#include <cassert>
#include <cmath>
#include <cstddef>
#include <immintrin.h> // SIMD intrinsics
#include <utility>

namespace x11engine::math {

constexpr float pi = 3.14159265358979323846f;

constexpr float radians(float degrees) noexcept {
    return degrees * (pi / 180.0f);
}

constexpr float degrees(float radians) noexcept {
    return radians * (180.0f / pi);
}

struct Vec2 {
    float x, y;
};

struct Vec3 {
    float x, y, z;

    constexpr Vec3() noexcept : x(0), y(0), z(0) {}
    constexpr Vec3(float x_, float y_, float z_) noexcept : x(x_), y(y_), z(z_) {}

    constexpr float &operator[](std::size_t index) noexcept {
        assert(index < 3);
        if (index == 0)
            return x;
        if (index == 1)
            return y;
        return z;
    }

    constexpr const float &operator[](std::size_t index) const noexcept {
        assert(index < 3);
        if (index == 0)
            return x;
        if (index == 1)
            return y;
        return z;
    }

    // Unary
    constexpr Vec3 operator-() const noexcept { return {-x, -y, -z}; }

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
        float inv = 1.0f / s;
        x *= inv;
        y *= inv;
        z *= inv;
        return *this;
    }
};

constexpr Vec3 operator+(Vec3 a, const Vec3 &b) noexcept { return a += b; }
constexpr Vec3 operator-(Vec3 a, const Vec3 &b) noexcept { return a -= b; }
constexpr Vec3 operator*(Vec3 v, float s) noexcept { return v *= s; }
constexpr Vec3 operator*(float s, Vec3 v) noexcept { return v *= s; }
constexpr Vec3 operator/(Vec3 v, float s) noexcept { return v /= s; }

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
    return len > 0.0f ? v / len : Vec3{0, 0, 0};
}

// Vec4 (SIMD Optimized)
struct alignas(16) Vec4 {
    // Anonymous union for SIMD access vs member access
    union {
        struct {
            float x, y, z, w;
        };
        __m128 mm;
    };

    constexpr Vec4() noexcept : x(0), y(0), z(0), w(0) {}

    constexpr Vec4(float x_, float y_, float z_, float w_) noexcept
        : x(x_), y(y_), z(z_), w(w_) {}

    // SIMD Constructor
    Vec4(__m128 m) noexcept : mm(m) {}

    constexpr float &operator[](std::size_t index) noexcept {
        assert(index < 4);
        if (index == 0)
            return x;
        if (index == 1)
            return y;
        if (index == 2)
            return z;
        return w;
    }

    constexpr const float &operator[](std::size_t index) const noexcept {
        assert(index < 4);
        if (index == 0)
            return x;
        if (index == 1)
            return y;
        if (index == 2)
            return z;
        return w;
    }
};

inline Vec4 operator+(const Vec4 &a, const Vec4 &b) noexcept {
    if consteval {
        return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
    } else {
        return _mm_add_ps(a.mm, b.mm);
    }
}

inline Vec4 operator-(const Vec4 &a, const Vec4 &b) noexcept {
    if consteval {
        return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
    } else {
        return _mm_sub_ps(a.mm, b.mm);
    }
}

inline Vec4 operator*(const Vec4 &v, float s) noexcept {
    if consteval {
        return {v.x * s, v.y * s, v.z * s, v.w * s};
    } else {
        return _mm_mul_ps(v.mm, _mm_set1_ps(s));
    }
}

inline Vec4 operator*(float s, const Vec4 &v) noexcept {
    return v * s;
}

// Mat4 (SIMD Optimized)
struct alignas(16) Mat4 {
    Vec4 c0, c1, c2, c3;

    constexpr Mat4() noexcept : c0{}, c1{}, c2{}, c3{} {}

    constexpr Mat4(const Vec4 &a, const Vec4 &b, const Vec4 &c, const Vec4 &d) noexcept : c0(a), c1(b), c2(c), c3(d) {}

    constexpr Vec4 &operator[](std::size_t index) noexcept {
        assert(index < 4);
        if (index == 0)
            return c0;
        if (index == 1)
            return c1;
        if (index == 2)
            return c2;
        return c3;
    }

    constexpr const Vec4 &operator[](std::size_t index) const noexcept {
        assert(index < 4);
        if (index == 0)
            return c0;
        if (index == 1)
            return c1;
        if (index == 2)
            return c2;
        return c3;
    }

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

    static Mat4 rotationX(float angleRadians) noexcept {
        float c = std::cos(angleRadians);
        float s = std::sin(angleRadians);
        return {
            {1, 0, 0, 0},
            {0, c, s, 0},
            {0, -s, c, 0},
            {0, 0, 0, 1}};
    }

    static Mat4 rotationY(float angleRadians) noexcept {
        float c = std::cos(angleRadians);
        float s = std::sin(angleRadians);
        return {
            {c, 0, -s, 0},
            {0, 1, 0, 0},
            {s, 0, c, 0},
            {0, 0, 0, 1}};
    }

    static Mat4 rotationZ(float angleRadians) noexcept {
        float c = std::cos(angleRadians);
        float s = std::sin(angleRadians);
        return {
            {c, s, 0, 0},
            {-s, c, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}};
    }
};

// SIMD Matrix-Vector Multiplication
// Result = c0*v.x + c1*v.y + c2*v.z + c3*v.w
inline Vec4 operator*(const Mat4 &m, const Vec4 &v) noexcept {
    if consteval {
        return {
            m.c0.x * v.x + m.c1.x * v.y + m.c2.x * v.z + m.c3.x * v.w,
            m.c0.y * v.x + m.c1.y * v.y + m.c2.y * v.z + m.c3.y * v.w,
            m.c0.z * v.x + m.c1.z * v.y + m.c2.z * v.z + m.c3.z * v.w,
            m.c0.w * v.x + m.c1.w * v.y + m.c2.w * v.z + m.c3.w * v.w};
    } else {
        // Use m.c0.mm instead of m.cols[0]
        __m128 res = _mm_mul_ps(m.c0.mm, _mm_set1_ps(v.x));
        res = _mm_add_ps(res, _mm_mul_ps(m.c1.mm, _mm_set1_ps(v.y)));
        res = _mm_add_ps(res, _mm_mul_ps(m.c2.mm, _mm_set1_ps(v.z)));
        res = _mm_add_ps(res, _mm_mul_ps(m.c3.mm, _mm_set1_ps(v.w)));
        return Vec4(res);
    }
}

// SIMD Matrix-Matrix Multiplication
inline Mat4 operator*(const Mat4 &a, const Mat4 &b) noexcept {
    // We unroll the loop here, which is safer than relying on pointer casts and just as fast (often faster due to pipelining).
    Mat4 res;
    res.c0 = a * b.c0;
    res.c1 = a * b.c1;
    res.c2 = a * b.c2;
    res.c3 = a * b.c3;
    return res;
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

/*
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

    static Mat4 rotationX(float angleRadians) noexcept {
        float c = std::cos(angleRadians);
        float s = std::sin(angleRadians);
        return {
            {1, 0, 0, 0},
            {0, c, s, 0},
            {0, -s, c, 0},
            {0, 0, 0, 1}};
    }

    static Mat4 rotationY(float angleRadians) noexcept {
        float c = std::cos(angleRadians);
        float s = std::sin(angleRadians);
        return {
            {c, 0, -s, 0},
            {0, 1, 0, 0},
            {s, 0, c, 0},
            {0, 0, 0, 1}};
    }

    static Mat4 rotationZ(float angleRadians) noexcept {
        float c = std::cos(angleRadians);
        float s = std::sin(angleRadians);
        return {
            {c, s, 0, 0},
            {-s, c, 0, 0},
            {0, 0, 1, 0},
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

*/
