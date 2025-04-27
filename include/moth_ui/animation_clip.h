#pragma once

#include "moth_ui/moth_ui.h"

#include <string>
#include <nlohmann/json.hpp>

namespace moth_ui {
    struct MOTH_UI_API AnimationClip {
        AnimationClip() = default;

        enum class LoopType {
            Stop,
            Loop,
            Reset,
        };

        std::string m_name;
        int m_startFrame = 0;
        int m_endFrame = 0;
        float m_fps = 30;
        LoopType m_loopType = LoopType::Stop;

        int FrameCount() const { return m_endFrame - m_startFrame; }

        bool operator==(AnimationClip const& other) const {
            return m_name == other.m_name && m_startFrame == other.m_startFrame && m_endFrame == other.m_endFrame && m_fps == other.m_fps && m_loopType == other.m_loopType;
        }

        bool operator!=(AnimationClip const& other) const {
            return !(*this == other);
        }

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnimationClip, m_name, m_startFrame, m_endFrame, m_fps, m_loopType);
    };
}
