#pragma once

#include "moth_ui/utils/interp.h"
#include "moth_ui/ui_fwd.h"

#include <nlohmann/json_fwd.hpp>

namespace moth_ui {
    class Keyframe {
    public:
        Keyframe() = default;
        Keyframe(int frame, KeyframeValue value)
            : m_frame(frame)
            , m_value(value) {}

        int m_frame = 0;
        KeyframeValue m_value = 0;
        InterpType m_interpType = InterpType::Linear;

        friend void to_json(nlohmann::json& j, Keyframe const& keyframe);
        friend void from_json(nlohmann::json const& j, Keyframe& keyframe);
    };
}
