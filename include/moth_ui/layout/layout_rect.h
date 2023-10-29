#pragma once

#include "moth_ui/utils/rect.h"
#include "moth_ui/utils/rect_serialization.h"

namespace moth_ui {
    struct LayoutRect {
        FloatRect anchor;
        FloatRect offset;
    };

    inline LayoutRect MakeDefaultLayoutRect() {
        LayoutRect rect;
        rect.anchor.topLeft = { 0.0f, 0.0f };
        rect.anchor.bottomRight = { 1.0f, 1.0f };
        rect.offset.topLeft = { 0.0f, 0.0f };
        rect.offset.bottomRight = { 0.0f, 0.0f };
        return rect;
    }

    inline LayoutRect& operator+=(LayoutRect& a, LayoutRect const& b) {
        a.anchor += b.anchor;
        a.offset += b.offset;
        return a;
    }

    inline LayoutRect& operator-=(LayoutRect& a, LayoutRect const& b) {
        a.anchor -= b.anchor;
        a.offset -= b.offset;
        return a;
    }

    inline LayoutRect operator+(LayoutRect const& a, LayoutRect const& b) {
        LayoutRect r = a;
        r += b;
        return r;
    }

    inline LayoutRect operator-(LayoutRect const& a, LayoutRect const& b) {
        LayoutRect r = a;
        r -= b;
        return r;
    }

    inline bool operator==(LayoutRect const& a, LayoutRect const& b) {
        return a.anchor == b.anchor && a.offset == b.offset;
    }

    inline bool operator!=(LayoutRect const& a, LayoutRect const& b) {
        return !(a == b);
    }

    inline void to_json(nlohmann::json& j, LayoutRect const& rect) {
        j = nlohmann::json{ { "anchor", rect.anchor }, { "offset", rect.offset } };
    }

    inline void from_json(nlohmann::json const& j, LayoutRect& rect) {
        rect.anchor = j.value("anchor", rect.anchor);
        rect.offset = j.value("offset", rect.offset);
    }
}
