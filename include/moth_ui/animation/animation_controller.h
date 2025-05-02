#pragma once

#include "moth_ui/ui_fwd.h"
#include "moth_ui/animation/animation_track_controller.h"

#include <vector>
#include <memory>

namespace moth_ui {
    class AnimationController {
    public:
        AnimationController(Node* node);

        void SetFrame(float frame);

    private:
        Node* m_node = nullptr;
        std::vector<std::unique_ptr<AnimationTrackController>> m_trackControllers;
    };
}
