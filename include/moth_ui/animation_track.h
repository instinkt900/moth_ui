#pragma once

#include "moth_ui/ui_fwd.h"

#include <nlohmann/json_fwd.hpp>

#include <array>
#include <functional>

namespace moth_ui {
    class AnimationTrack {
    public:
        enum class Target {
            Unknown,
            TopOffset,
            BottomOffset,
            LeftOffset,
            RightOffset,
            TopAnchor,
            BottomAnchor,
            LeftAnchor,
            RightAnchor,
            ColorRed,
            ColorGreen,
            ColorBlue,
            ColorAlpha,
        };

        static constexpr std::array<Target, 12> ContinuousTargets{
            Target::TopOffset,
            Target::BottomOffset,
            Target::LeftOffset,
            Target::RightOffset,
            Target::TopAnchor,
            Target::BottomAnchor,
            Target::LeftAnchor,
            Target::RightAnchor,
            Target::ColorRed,
            Target::ColorGreen,
            Target::ColorBlue,
            Target::ColorAlpha,
        };

        AnimationTrack() = default;
        AnimationTrack(AnimationTrack const& other);
        explicit AnimationTrack(Target target);
        AnimationTrack(Target target, float initialValue);
        explicit AnimationTrack(nlohmann::json const& json);

        Target GetTarget() const { return m_target; }

        Keyframe* GetKeyframe(int frameNo);
        Keyframe& GetOrCreateKeyframe(int frameNo);
        void DeleteKeyframe(int frameNo);
        void DeleteKeyframe(Keyframe* frame);

        void ForKeyframesOverFrames(float startFrame, float endFrame, std::function<void(Keyframe const&)> const& callback);

        float GetValueAtFrame(float frame) const;

        void SortKeyframes();

        friend void to_json(nlohmann::json& j, AnimationTrack const& track);
        friend void from_json(nlohmann::json const& j, AnimationTrack& track);

        std::vector<std::shared_ptr<Keyframe>> m_keyframes;

    private:
        Target m_target = Target::Unknown;
    };
}
