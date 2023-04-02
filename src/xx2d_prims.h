#pragma once
#include "xx2d.h"

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

        Pos MakeAdd(IsArithmetic auto const& vx, IsArithmetic auto const& vy) const {
            return { x + vx, y + vy };
        }

        void Set(HasFieldXY auto const& v) {
            x = T(v.x);
            y = T(v.y);
        }

        template<typename U = float>
        auto As() const -> Pos<U> {
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

    // 4 floats color
    struct RGBA {
        float r, g, b, a;

        operator RGBA8() const {
            return { uint8_t(r * 255), uint8_t(g * 255), uint8_t(b * 255), uint8_t(a * 255) };
        }

        RGBA operator+(RGBA const& v) const {
            return { r + v.r, g + v.g, b + v.b, a + v.a };
        }
        RGBA operator-(RGBA const& v) const {
            return { r - v.r, g - v.g, b - v.b, a - v.a };
        }

        RGBA operator*(IsArithmetic auto const& v) const {
            return { r * v, g * v, b * v, a * v };
        }
        RGBA operator/(IsArithmetic auto const& v) const {
            return { r / v, g / v, b / v, a / v };
        }

        RGBA& operator+=(RGBA const& v) {
            r += v.r;
            g += v.g;
            b += v.b;
            a += v.a;
            return *this;
        }
    };

    // pos + size
    struct Rect : XY {
        XY wh;
    };


    // for node, cam, ... 
    // reference from cocos 2.x AffineTransform
    struct AffineTransform {
        float a, b, c, d;
        float tx, ty;

        static AffineTransform MakePosScaleRadiansAnchorSize(XY const& pos, XY const& scale, float const& radians, XY const& anchorSize);
        static AffineTransform MakePosScaleRadians(XY const& pos, XY const& scale, float const& radians);
        static AffineTransform MakePosScale(XY const& pos, XY const& scale);
        static AffineTransform MakePos(XY const& pos);

        AffineTransform MakeConcat(AffineTransform const& t) const;
        AffineTransform MakeInvert(AffineTransform const& t);

        static AffineTransform MakeIdentity();
        AffineTransform& Translate(XY const& pos);
        AffineTransform& Scale(XY const& scale);
        AffineTransform& Rotate(float const& radians);

        XY Apply(XY const& point) const;
    };
}
