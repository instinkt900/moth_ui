#include "common.h"
#include "add_keyframe_action.h"
#include "moth_ui/layout/layout_entity.h"

AddKeyframeAction::AddKeyframeAction(std::shared_ptr<moth_ui::LayoutEntity> entity, moth_ui::AnimationTrack::Target target, int frameNo, moth_ui::KeyframeValue value, moth_ui::InterpType interp)
    : m_entity(entity)
    , m_target(target)
    , m_frameNo(frameNo)
    , m_value(value)
    , m_interp(interp) {
}

AddKeyframeAction::~AddKeyframeAction() {
}

void AddKeyframeAction::Do() {
    auto& track = m_entity->m_tracks.at(m_target);
    auto& keyframe = track->GetOrCreateKeyframe(m_frameNo);
    keyframe.m_value = m_value;
    keyframe.m_interpType = m_interp;
}

void AddKeyframeAction::Undo() {
    auto& track = m_entity->m_tracks.at(m_target);
    track->DeleteKeyframe(m_frameNo);
}

void AddKeyframeAction::OnImGui() {
    if (ImGui::CollapsingHeader("AddKeyframeAction")) {
        ImGui::LabelText("Frame", "%d", m_frameNo);
        ImGui::LabelText("Value", "%f", m_value);
    }
}
