#pragma once

#include "moth_ui/ui_fwd.h"

#include <nlohmann/json_fwd.hpp>

#include <string>

namespace moth_ui {
    class AnimationEvent {
    public:
        AnimationEvent() = default;
        AnimationEvent(int frame, std::string const& name)
            : m_frame(frame)
            , m_name(name) {}

        int m_frame = 0;
        std::string m_name;

        bool operator==(AnimationEvent const& other) const {
            return m_frame == other.m_frame && m_name == other.m_name;
        }

        bool operator!=(AnimationEvent const& other) const {
            return !(*this == other);
        }

        friend void to_json(nlohmann::json& j, AnimationEvent const& event);
        friend void from_json(nlohmann::json const& j, AnimationEvent& event);
    };
}
