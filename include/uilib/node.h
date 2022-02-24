#pragma once

#include "event_listener.h"
#include "layout/layout_types.h"
#include "ui_fwd.h"

namespace ui {
    class Node : public EventListener {
    public:
        Node();
        Node(std::shared_ptr<LayoutEntity> layoutEntity);
        virtual ~Node();

        virtual bool OnEvent(Event const& event) override;
        virtual void Update(uint32_t ticks);
        virtual void Draw(SDL_Renderer& renderer);

        virtual void SendEvent(Event const& event);

        void SetId(std::string const& id) { m_id = id; }
        std::string const& GetId() const { return m_id; }

        void SetVisible(bool visible) { m_visible = visible; }
        bool IsVisible() const { return m_visible; }

        void SetShowRect(bool show) { m_showRect = show; }
        bool GetShowRect() const { return m_showRect; }

        void SetParent(Group* parent) { m_parent = parent; }
        Group* GetParent() const { return m_parent; }

        LayoutRect& GetLayoutRect() { return m_layoutRect; }
        IntRect const& GetScreenRect() const { return m_screenRect; }
        void SetScreenRect(IntRect const& rect);
        virtual void UpdateChildBounds() {}

        void RefreshBounds(int frameNo);
        void RecalculateBounds();
        bool IsInBounds(IntVec2 const& point) const;
        IntVec2 TranslatePosition(IntVec2 const& point) const;

        virtual bool SetAnimation(std::string const& name) { return false; }
        void SetAnimationClip(AnimationClip* clip);
        virtual void UpdateAnimTime(float delta);

        using EventHandler = std::function<bool(Node*, Event const&)>;
        void SetEventHandler(EventHandler const& handler) { m_eventHandler = handler; }

        auto GetLayoutEntity() const { return m_layout; }

        virtual void DebugDraw();

    protected:
        std::shared_ptr<LayoutEntity> m_layout;

        Group* m_parent = nullptr;

        std::string m_id;
        LayoutRect m_layoutRect;

        bool m_visible = true;
        bool m_showRect = false;
        bool m_overrideScreenRect = false;
        IntRect m_screenRect;

        std::unique_ptr<AnimationController> m_animationController;
        EventHandler m_eventHandler;
    };
}
