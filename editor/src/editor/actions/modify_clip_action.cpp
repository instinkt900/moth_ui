#include "common.h"
#include "modify_clip_action.h"

namespace ui {
    ModifyClipAction::ModifyClipAction(AnimationClip* target, AnimationClip const& initialValues)
        : m_target(target)
        , m_initialValues(initialValues)
        , m_finalValues(*m_target) {
    }

    void ModifyClipAction::Do() {
        *m_target = m_finalValues;
    }

    void ModifyClipAction::Undo() {
        *m_target = m_initialValues;
    }

    void ModifyClipAction::OnImGui() {
        if (ImGui::CollapsingHeader("ModifyClipAction")) {
        }
    }
}
