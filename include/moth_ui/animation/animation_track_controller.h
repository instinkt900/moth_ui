#pragma once

#include "moth_ui/animation/animation_track.h"
#include "moth_ui/ui_fwd.h"

namespace moth_ui {
    class AnimationTrackController {
    public:
        AnimationTrackController(float& target, AnimationTrack& track);

        auto GetTarget() const { return m_track.GetTarget(); }
        void SetFrame(float frame);

    private:
        float& m_target;
        AnimationTrack& m_track;
    };
}
