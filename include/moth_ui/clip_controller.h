#pragma once

#include "moth_ui/moth_ui.h"
#include "moth_ui/node.h"
#include "moth_ui/animation_clip.h"

namespace moth_ui {
    class MOTH_UI_API ClipController {
    public:
        Node* m_node = nullptr;
        AnimationClip* m_clip = nullptr;
    };
}
