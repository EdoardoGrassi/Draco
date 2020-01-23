#pragma once
#ifndef DRAKO_TRANSFORM_HPP
#define DRAKO_TRANSFORM_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/graphics/camera.hpp"
#include "drako/math/mat4x4.hpp"
#include "drako/math/quaternion.hpp"
#include "drako/math/vector3.hpp"

namespace drako
{
    // Creates a translation transform matrix.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4
    translate(float _x_, float _y_, float _z_) noexcept
    {
        return mat4x4({1.f, 0.f, 0.f, _x_,
            0.f, 1.f, 0.f, _y_,
            0.f, 0.f, 1.f, _z_,
            0.f, 0.f, 0.f, 1.f});
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4
    translate(vec3 v) noexcept
    {
        return translate(v[0], v[1], v[2]);
    }

    // Creates a rotation transform matrix.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4 rotate(float x, float y, float z) noexcept;
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4 rotate(uquat rotation) noexcept;

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4
    rotate_x(float radians) noexcept(std::is_nothrow_constructible_v<mat4x4, std::array<float, 16>>)
    {
        const auto cos = std::cos(radians);
        const auto sin = std::sin(radians);
        return mat4x4({1.f, 0.f, 0.f, 0.f,
            0.f, cos, -sin, 0.f,
            0.f, sin, cos, 0.f,
            0.f, 0.f, 0.f, 1.f});
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4
    rotate_y(float radians) noexcept(std::is_nothrow_constructible_v<mat4x4, std::array<float, 16>>)
    {
        const auto cos = std::cos(radians);
        const auto sin = std::sin(radians);
        return mat4x4{{cos, 0.f, sin, 0.f},
            {0.f, 1.f, 0.f, 0.f},
            {-sin, 0.f, cos, 0.f},
            {0.f, 0.f, 0.f, 1.f}};
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4
    rotate_z(float radians) noexcept(std::is_nothrow_constructible_v<mat4x4, std::array<float, 16>>)
    {
        const auto cos = std::cos(radians);
        const auto sin = std::sin(radians);
        return mat4x4({cos, -sin, 0.f, 0.f,
            sin, cos, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f});
    }



    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4
    scale(float _x_, float _y_, float _z_) noexcept
    {
        return mat4x4({_x_, 0.f, 0.f, 0.f,
            0.f, _y_, 0.f, 0.f,
            0.f, 0.f, _z_, 0.f,
            0.f, 0.f, 0.f, 1.f});
    }

    // Creates a scaling transform matrix.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4 scale(float s) noexcept
    {
        return scale(s, s, s);
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4 scale(const vec3& s) noexcept
    {
        return scale(s[0], s[1], s[2]);
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4
    shearing_xy(float _x_, float _y_) noexcept
    {
        return mat4x4({1.f, 0.f, _x_, 0.f,
            0.f, 1.f, _y_, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f});
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4
    shearing_yz(float _y_, float _z_) noexcept
    {
        return mat4x4({1.f, 0.f, 0.f, 0.f,
            _y_, 1.f, 0.f, 0.f,
            _z_, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f});
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4
    shearing_xz(float _x_, float _z_) noexcept
    {
        return mat4x4({1.f, _x_, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, _z_, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f});
    }

    // Creates a translation, rotation and scaling transform matrix.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4
    transform(vec3 p, uquat r, vec3 s) noexcept
    {
    }

    // Creates an ortographic projection matrix.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4
    ortographic(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max) noexcept
    {
        const float sxx = 2.f / (x_max - x_min); // normalize x in range [-1, 1]
        const float syy = 2.f / (y_max - y_min); // normalize y in range [-1, 1]
        const float szz = 1.f / (z_max - z_min); // normalize z in range [0, 1]

        auto S = mat4x4({sxx, 0.f, 0.f, 0.f,
            0.f, syy, 0.f, 0.f,
            0.f, 0.f, szz, 0.f,
            0.f, 0.f, 0.f, 1.f});

        // translate to origin (0,0,0)
        const float txx = -(x_max + x_min) / 2.f;
        const float tyy = -(y_max + y_min) / 2.f;
        const float tzz = -(z_max + z_min) / 2.f;

        auto T = mat4x4({1.f, 0.f, 0.f, txx,
            0.f, 1.f, 0.f, tyy,
            0.f, 0.f, 1.f, tzz,
            0.f, 0.f, 0.f, 1.f});

        return S * T;
        /*
        return mat4x4({ 1.f, 0.f, 0.f, 0.f,
                        0.f, 1.f, 0.f, 0.f,
                        0.f, 0.f, 0.f, 0.f,
                        0.f, 0.f, 0.f, 1.f});
                        */
    }

    // Creates a perspective projection matrix.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4
    perspective(float l, float r, float b, float t, float n, float f) noexcept
    {
        const float sxx = 2 * n / (r - l);
        const float syy = 2 * n / (t - b);
        const float szz = f / (f - n);
        const float sww = -f * n / (f - n);

        const float txx = -(r + l) / (r - l);
        const float tyy = -(t + b) / (t - b);

        return mat4x4({sxx, 0.f, txx, 0.f,
            0.f, syy, tyy, 0.f,
            0.f, 0.f, szz, sww,
            0.f, 0.f, 1.f, 1.f});
    }

    // Creates a perspective projection matrix.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr mat4x4
    perspective(const gpx::camera_frustum& f) noexcept
    {
        const float sxx = 2 * f.zmin / (f.xmax - f.xmin);
        const float syy = 2 * f.zmin / (f.ymax - f.ymin);
        const float szz = f.zmax / (f.zmax - f.zmin);
        const float sww = -f.zmax * f.zmin / (f.zmax - f.zmin);

        const float txx = -(f.xmax + f.xmin) / (f.xmax - f.xmin);
        const float tyy = -(f.ymax + f.ymin) / (f.ymax - f.ymin);

        return mat4x4({sxx, 0.f, txx, 0.f,
            0.f, syy, tyy, 0.f,
            0.f, 0.f, szz, sww,
            0.f, 0.f, 1.f, 1.f});
    }

    // Rotates about axis passing through point in world space by angle degrees.
    DRAKO_NODISCARD constexpr mat4x4
    rotate_around(vec3 point, vec3 axis, float angle) noexcept;

    DRAKO_NODISCARD constexpr mat4x4
    look_at(vec3 camera, vec3 target, norm3 up) noexcept;

} // namespace drako

#endif // !DRAKO_TRANSFORM_HPP