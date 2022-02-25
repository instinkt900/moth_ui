#include "common.h"
#include "moth_ui/layout/layout_entity.h"
#include "moth_ui/layout/layout_entity_group.h"
#include "moth_ui/node.h"
#include "moth_ui/utils/imgui_ext_inspect.h"

namespace ui {
    LayoutEntity::LayoutEntity(LayoutRect const& initialBounds) {
        InitTracks(initialBounds);
    }

    LayoutEntity::LayoutEntity(LayoutEntityGroup* parent)
        : m_parent(parent) {
    }

    LayoutEntity::LayoutEntity(nlohmann::json const& json, LayoutEntityGroup* parent)
        : m_parent(parent) {
        Deserialize(json);
    }

    void LayoutEntity::SetBounds(LayoutRect const& bounds, int frame) {
        auto SetValue = [&](AnimationTrack::Target target, float value) {
            auto track = m_tracks.at(target);
            auto& keyframe = track->GetOrCreateKeyframe(frame);
            keyframe.m_value = value;
        };

        SetValue(AnimationTrack::Target::LeftAnchor, bounds.anchor.topLeft.x);
        SetValue(AnimationTrack::Target::TopAnchor, bounds.anchor.topLeft.y);
        SetValue(AnimationTrack::Target::RightAnchor, bounds.anchor.bottomRight.x);
        SetValue(AnimationTrack::Target::BottomAnchor, bounds.anchor.bottomRight.y);
        SetValue(AnimationTrack::Target::LeftOffset, bounds.offset.topLeft.x);
        SetValue(AnimationTrack::Target::TopOffset, bounds.offset.topLeft.y);
        SetValue(AnimationTrack::Target::RightOffset, bounds.offset.bottomRight.x);
        SetValue(AnimationTrack::Target::BottomOffset, bounds.offset.bottomRight.y);
    }

    LayoutRect LayoutEntity::GetBoundsAtTime(float time) const {
        auto GetValue = [&](AnimationTrack::Target target) {
            float value = 0;
            auto const trackIt = m_tracks.find(target);
            if (std::end(m_tracks) != trackIt) {
                const auto track = trackIt->second;
                value = track->GetValueAtTime(time);
            }
            return value;
        };

        LayoutRect bounds;
        bounds.anchor.topLeft.x = GetValue(AnimationTrack::Target::LeftAnchor);
        bounds.anchor.topLeft.y = GetValue(AnimationTrack::Target::TopAnchor);
        bounds.anchor.bottomRight.x = GetValue(AnimationTrack::Target::RightAnchor);
        bounds.anchor.bottomRight.y = GetValue(AnimationTrack::Target::BottomAnchor);
        bounds.offset.topLeft.x = GetValue(AnimationTrack::Target::LeftOffset);
        bounds.offset.topLeft.y = GetValue(AnimationTrack::Target::TopOffset);
        bounds.offset.bottomRight.x = GetValue(AnimationTrack::Target::RightOffset);
        bounds.offset.bottomRight.y = GetValue(AnimationTrack::Target::BottomOffset);
        return bounds;
    }

    LayoutRect LayoutEntity::GetBoundsAtFrame(int frame) const {
        auto GetValue = [&](AnimationTrack::Target target) {
            float value = 0;
            auto const trackIt = m_tracks.find(target);
            if (std::end(m_tracks) != trackIt) {
                const auto track = trackIt->second;
                value = track->GetValueAtFrame(frame);
            }
            return value;
        };

        LayoutRect bounds;
        bounds.anchor.topLeft.x = GetValue(AnimationTrack::Target::LeftAnchor);
        bounds.anchor.topLeft.y = GetValue(AnimationTrack::Target::TopAnchor);
        bounds.anchor.bottomRight.x = GetValue(AnimationTrack::Target::RightAnchor);
        bounds.anchor.bottomRight.y = GetValue(AnimationTrack::Target::BottomAnchor);
        bounds.offset.topLeft.x = GetValue(AnimationTrack::Target::LeftOffset);
        bounds.offset.topLeft.y = GetValue(AnimationTrack::Target::TopOffset);
        bounds.offset.bottomRight.x = GetValue(AnimationTrack::Target::RightOffset);
        bounds.offset.bottomRight.y = GetValue(AnimationTrack::Target::BottomOffset);
        return bounds;
    }

    std::unique_ptr<Node> LayoutEntity::Instantiate() {
        return std::make_unique<Node>(shared_from_this());
    }

    void LayoutEntity::OnEditDraw() {
        if (ImGui::TreeNode("node")) {
            imgui_ext::Inspect("id", m_id);
            //ImGuiInspectMember("bounds", m_bounds);
            ImGui::TreePop();
        }
    }

    void LayoutEntity::InitTracks(LayoutRect const& initialRect) {
        auto const targetList = {
            std::make_pair(AnimationTrack::Target::TopAnchor, initialRect.anchor.topLeft.y),
            std::make_pair(AnimationTrack::Target::LeftAnchor, initialRect.anchor.topLeft.x),
            std::make_pair(AnimationTrack::Target::BottomAnchor, initialRect.anchor.bottomRight.y),
            std::make_pair(AnimationTrack::Target::RightAnchor, initialRect.anchor.bottomRight.x),
            std::make_pair(AnimationTrack::Target::TopOffset, initialRect.offset.topLeft.y),
            std::make_pair(AnimationTrack::Target::LeftOffset, initialRect.offset.topLeft.x),
            std::make_pair(AnimationTrack::Target::BottomOffset, initialRect.offset.bottomRight.y),
            std::make_pair(AnimationTrack::Target::RightOffset, initialRect.offset.bottomRight.x),
        };
        if (std::end(m_tracks) == m_tracks.find(AnimationTrack::Target::Events)) {
            m_tracks.insert(std::make_pair(AnimationTrack::Target::Events, std::make_shared<AnimationTrack>(AnimationTrack::Target::Events)));
        }
        for (auto&& [target, value] : targetList) {
            if (std::end(m_tracks) == m_tracks.find(target)) {
                m_tracks.insert(std::make_pair(target, std::make_shared<AnimationTrack>(target, value)));
            }
        }
    }

    nlohmann::json LayoutEntity::Serialize() const {
        nlohmann::json j;
        j["type"] = GetType();
        j["m_id"] = m_id;
        nlohmann::json trackJson;
        for (auto&& [target, track] : m_tracks) {
            trackJson.push_back(*track);
        }
        j["m_tracks"] = trackJson;
        return j;
    }

    nlohmann::json LayoutEntity::SerializeAsChild() const {
        return Serialize();
    }

    void LayoutEntity::Deserialize(nlohmann::json const& json) {
        auto const type = GetType();
        assert(json["type"] == type);
        json["m_id"].get_to(m_id);
        if (json.contains("m_tracks")) {
            m_tracks.clear();
            auto const* animationClips = m_parent ? &m_parent->GetAnimationClips() : nullptr;
            auto const& tracksJson = json["m_tracks"];
            for (auto&& trackJson : tracksJson) {
                auto track = std::make_unique<AnimationTrack>(trackJson);
                if (animationClips) {
                    track->UpdateTrackTimings(*animationClips);
                }
                m_tracks.insert(std::make_pair(track->GetTarget(), std::move(track)));
            }
        }
    }
}
