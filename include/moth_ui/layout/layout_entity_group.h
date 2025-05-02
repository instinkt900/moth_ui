#pragma once

#include "moth_ui/layout/layout_entity.h"
#include "moth_ui/animation/animation_clip.h"
#include "moth_ui/animation/animation_event.h"

#include <nlohmann/json_fwd.hpp>

namespace moth_ui {
    class LayoutEntityGroup : public LayoutEntity {
    public:
        explicit LayoutEntityGroup(LayoutRect const& initialBounds);
        explicit LayoutEntityGroup(LayoutEntityGroup* parent);
        LayoutEntityGroup(LayoutEntityGroup const& other);

        LayoutEntityType GetType() const override { return LayoutEntityType::Group; }

        nlohmann::json Serialize(SerializeContext const& context) const override;
        bool Deserialize(nlohmann::json const& json, SerializeContext const& context) override;

        std::vector<std::shared_ptr<LayoutEntity>> m_children;
        std::vector<std::unique_ptr<AnimationClip>> m_clips;
        std::vector<std::unique_ptr<AnimationEvent>> m_events;
    };
}
