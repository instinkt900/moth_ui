#pragma once

#include "vec2.h"

namespace moth_ui {
    template <typename T>
    struct Rect {
        Vec2<T> topLeft;
        Vec2<T> bottomRight;
    };

    using IntRect = Rect<int>;
    using FloatRect = Rect<float>;

    template <typename T>
    inline bool operator==(Rect<T> const& a, Rect<T> const& b) {
        return (a.topLeft == b.topLeft) && (a.bottomRight == b.bottomRight);
    }

    template <typename T>
    inline bool operator!=(Rect<T> const& a, Rect<T> const& b) {
        return !(a == b);
    }

    template <typename T>
    inline Rect<T>& operator+=(Rect<T>& a, Rect<T> const& b) {
        a.topLeft += b.topLeft;
        a.bottomRight += b.bottomRight;
        return a;
    }

    template <typename T>
    inline Rect<T>& operator-=(Rect<T>& a, Rect<T> const& b) {
        a.topLeft -= b.topLeft;
        a.bottomRight -= b.bottomRight;
        return a;
    }

    template <typename T>
    inline void to_json(nlohmann::json& j, Rect<T> const& rect) {
        j = nlohmann::json{ { "topLeft", rect.topLeft }, { "bottomRight", rect.bottomRight } };
    }

    template <typename T>
    inline void from_json(nlohmann::json const& j, Rect<T>& rect) {
        j.at("topLeft").get_to(rect.topLeft);
        j.at("bottomRight").get_to(rect.bottomRight);
    }

    template <typename T, typename U>
    inline bool IsInRect(Vec2<T> const& point, Rect<U> const& rect) {
        if (point.x > rect.bottomRight.x || point.x < rect.topLeft.x ||
            point.y > rect.bottomRight.y || point.y < rect.topLeft.y) {
            return false;
        }
        return true;
    }
}
