#pragma once

#include "moth_ui/nodes/node.h"
#include "moth_ui/animation/animation_clip.h"

namespace moth_ui {
    class ClipController {
    public:
        Node* m_node = nullptr;
        AnimationClip* m_clip = nullptr;
    };
}
