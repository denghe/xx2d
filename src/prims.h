#pragma once
#include "pch.h"

namespace xx {

    // position ( screen center == 0, 0 )
    template<typename T>
    concept HasFieldXY = requires { T::x; T::y; };

    template<typename T>
    concept IsArithmetic = std::is_arithmetic_v<T>;

    // pos
    template<typename T = int32_t>
    struct Pos {
        T x, y;

        // -x
        Pos operator-() const {
            return { -x, -y };
        }

        // + - * /
        Pos operator+(HasFieldXY auto const& v) const {
            return { T(x + v.x), T(y + v.y) };
        }
        Pos operator-(HasFieldXY auto const& v) const {
            return { T(x - v.x), T(y - v.y) };
        }
        Pos operator*(HasFieldXY auto const& v) const {
            return { T(x * v.x), T(y * v.y) };
        }
        Pos operator/(HasFieldXY auto const& v) const {
            return { T(x / v.x), T(y / v.y) };
        }

        Pos operator+(IsArithmetic auto const& v) const {
            return { T(x + v), T(y + v) };
        }
        Pos operator-(IsArithmetic auto const& v) const {
            return { T(x - v), T(y - v) };
        }
        Pos operator*(IsArithmetic auto const& v) const {
            return { T(x * v), T(y * v) };
        }
        Pos operator/(IsArithmetic auto const& v) const {
            return { T(x / v), T(y / v) };
        }

        // += -= *= /=
        Pos& operator+=(HasFieldXY auto const& v) {
            x = T(x + v.x);
            y = T(y + v.y);
            return *this;
        }
        Pos& operator-=(HasFieldXY auto const& v) {
            x = T(x - v.x);
            y = T(y - v.y);
            return *this;
        }
        Pos& operator*=(HasFieldXY auto const& v) {
            x = T(x * v.x);
            y = T(y * v.y);
            return *this;
        }
        Pos& operator/=(HasFieldXY auto const& v) {
            x = T(x / v.x);
            y = T(y / v.y);
            return *this;
        }

        Pos& operator+=(IsArithmetic auto const& v) {
            x = T(x + v);
            y = T(y + v);
            return *this;
        }
        Pos operator-=(IsArithmetic auto const& v) {
            x = T(x - v);
            y = T(y - v);
            return *this;
        }
        Pos& operator*=(IsArithmetic auto const& v) {
            x = T(x * v);
            y = T(y * v);
            return *this;
        }
        Pos operator/=(IsArithmetic auto const& v) {
            x = T(x / v);
            y = T(y / v);
            return *this;
        }

        // == !=
        bool operator==(HasFieldXY auto const& v) const {
            return x == v.x && y == v.y;
        }
        bool operator!=(HasFieldXY auto const& v) const {
            return x != v.x || y != v.y;
        }

        void Set(HasFieldXY auto const& v) {
            x = T(v.x);
            y = T(v.y);
        }

        template<typename U = float>
        auto As() -> Pos<U> const {
            return { (U)x, (U)y };
        }

        bool IsZero() const {
            return x == T{} && y == T{};
        }

        void Clear() {
            x = {};
            y = {};
        }

        template<typename U = float>
        Pos& Normalize() {
            auto v = std::sqrt(U(x * x + y * y));
            assert(v);
            x = T(x / v);
            y = T(y / v);
            return *this;
        }
        template<typename R = T, typename U = float>
        auto MakeNormalize() -> Pos<R> {
            auto v = std::sqrt(U(x * x + y * y));
            assert(v);
            return { R(x / v), R(y / v) };
        }

        Pos& FlipY() {
            y = -y;
            return *this;
        }
        template<typename R = T>
        auto MakeFlipY() -> Pos<R> {
            return { R(x), R(-y) };
        }
    };
    using XY = Pos<float>;

    // texture uv mapping pos
    struct UV {
        uint16_t u, v;
    };

    // 4 bytes color
    struct RGBA8 {
        uint8_t r, g, b, a;
        bool operator==(RGBA8 const&) const = default;
        bool operator!=(RGBA8 const&) const = default;
    };

    // 1 vert data
    struct XYUVRGBA8 : XY, UV, RGBA8 {};

    /*
     1┌────┐2
      │    │
     0└────┘3
    */
    using QuadVerts = std::array<XYUVRGBA8, 4>;

    // 1 point data ( for draw line strip )
    struct XYRGBA8 : XY, RGBA8 {};



    //
    struct Size {
        float w, h;
        bool operator==(Size const&) const = default;
        bool operator!=(Size const&) const = default;

        inline Size operator+(Size const& o) const { return { w + o.w, h + o.h }; }
        inline Size operator-(Size const& o) const { return { w - o.w, h - o.h }; }
        inline Size operator-() const { return { -w, -h }; }
        inline Size operator*(float const& o) const { return { w * o, h * o }; }
        inline Size operator/(float const& o) const { return { w / o, h / o }; }
    };

    //
    struct Rect : XY, Size {};

    // for node, cam, ... ( maybe convert to 3 * 2/3 matrix ? )
    struct Translate {
        // todo: angle
        XY scale = { 1, 1 };
        XY offset = { 0, 0 };
    };


    // reference from cocos 2.x AffineTransform
    struct AffineTransform {
        float a, b, c, d;
        float tx, ty;

        static AffineTransform Make(XY const& pos, XY const& anchorSize, XY const& scale, float const& radians);
        static AffineTransform MakeIdentity();

        AffineTransform MakeConcat(AffineTransform const& t) const;
        AffineTransform MakeInvert(AffineTransform const& t);

        AffineTransform& Translate(XY const& pos);
        AffineTransform& Scale(XY const& scale);
        AffineTransform& Rotate(float const& radians);

        XY Apply(XY const& point) const;
    };

}
