#include "common.h"
#include "moth_ui/layout/layout_entity_group.h"
#include "moth_ui/animation/animation_clip.h"
#include "moth_ui/animation/animation_marker.h"
#include "moth_ui/nodes/group.h"

namespace moth_ui {
    LayoutEntityGroup::LayoutEntityGroup(LayoutRect const& initialBounds)
        : LayoutEntity(initialBounds) {
    }

    LayoutEntityGroup::LayoutEntityGroup(LayoutEntityGroup* parent)
        : LayoutEntity(parent) {
    }

    LayoutEntityGroup::LayoutEntityGroup(LayoutEntityGroup const& other)
        : LayoutEntity(other) {
        for (auto&& child : other.m_children) {
            m_children.push_back(child->Clone(moth_ui::LayoutEntity::CloneType::Deep));
        }
        for (auto&& clip : other.m_clips) {
            m_clips.push_back(std::make_shared<AnimationClip>(*clip));
        }
        for (auto&& event : other.m_events) {
            m_events.push_back(std::make_unique<AnimationMarker>(*event));
        }
    }

    LayoutEntityGroup::LayoutEntityGroup(LayoutEntityGroup&& other) noexcept
        : LayoutEntity(std::move(other))
        , m_children(std::move(other.m_children))
        , m_clips(std::move(other.m_clips))
        , m_events(std::move(other.m_events)) {
        for (auto& child : m_children) {
            child->m_parent = this;
        }
    }

    LayoutEntityGroup::~LayoutEntityGroup() = default;

    std::shared_ptr<LayoutEntity> LayoutEntityGroup::Clone(CloneType cloneType) {
        auto const cloned = std::make_shared<LayoutEntityGroup>(*this);
        if (cloneType == CloneType::Shallow) {
            // Deep copy and then discard, the way LayoutEntityRef does it. It is
            // easier than making sure every base class field is copied by hand.
            cloned->m_children.clear();
        }
        return cloned;
    }

    std::shared_ptr<Node> LayoutEntityGroup::Instantiate(Context& context) {
        return Group::Create(context, std::static_pointer_cast<LayoutEntityGroup>(shared_from_this()));
    }

    nlohmann::json LayoutEntityGroup::Serialize(SerializeContext const& context) const {
        nlohmann::json j = LayoutEntity::Serialize(context);
        j["clips"] = m_clips;
        j["events"] = m_events;

        std::vector<nlohmann::json> childJsons;
        for (auto&& child : m_children) {
            childJsons.push_back(child->Serialize(context));
        }
        j["children"] = childJsons;
        return j;
    }

    bool LayoutEntityGroup::Deserialize(nlohmann::json const& json, SerializeContext const& context) {
        if (!LayoutEntity::Deserialize(json, context)) {
            return false;
        }

        m_clips = json.value("clips", decltype(m_clips){});
        m_events = json.value("events", decltype(m_events){});

        m_children.clear();
        if (auto const childrenIt = json.find("children"); childrenIt != json.end()) {
            for (auto&& childJson : *childrenIt) {
                if (auto child = LoadEntity(childJson, this, context)) {
                    m_children.push_back(std::move(child));
                }
            }
        }
        return true;
    }
}
