#include "common.h"
#include "moth_ui/animation/animation_track_controller.h"

namespace moth_ui {
    AnimationTrackController::AnimationTrackController(float& target, AnimationTrack& track)
        : m_target(target)
        , m_track(track) {
    }

    void AnimationTrackController::SetFrame(float frame) {
        m_target = m_track.GetValueAtFrame(frame);
    }
}
