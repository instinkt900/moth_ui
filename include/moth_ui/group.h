#pragma once

#include "node.h"

namespace moth_ui {
    class Group : public Node {
    public:
        Group();
        Group(std::shared_ptr<LayoutEntityGroup> layoutEntityGroup);
        virtual ~Group();

        bool OnEvent(Event const& event) override;
        void Update(uint32_t ticks) override;

        void UpdateChildBounds() override;

        void AddChild(std::shared_ptr<Node> child);
        void RemoveChild(std::shared_ptr<Node> child);
        int GetChildCount() const { return static_cast<int>(m_children.size()); }
        auto& GetChildren() { return m_children; }
        auto const& GetChildren() const { return m_children; }

        bool SetAnimation(std::string const& name) override;
        void StopAnimation() override;

        void DebugDraw() override;

    protected:
        std::vector<std::shared_ptr<Node>> m_children;

        void DrawInternal() override;
    };
}
