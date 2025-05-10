#pragma once

#include "moth_ui/events/event_listener.h"
#include "moth_ui/utils/vector.h"
#include "moth_ui/ui_fwd.h"

#include <memory>
#include <vector>

namespace moth_ui {
    class LayerStack : public EventListener {
    public:
        LayerStack(IRenderer& renderer, int renderWidth, int renderHeight, int windowWidth, int windowHeight);

        void PushLayer(std::unique_ptr<Layer>&& layer);
        std::unique_ptr<Layer> PopLayer();
        void RemoveLayer(Layer* layer);

        bool OnEvent(Event const& event) override;
        void Update(uint32_t ticks);
        void Draw();
        void DebugDraw();

        void SetWindowSize(IntVec2 const& dimensions);
        void SetRenderSize(IntVec2 const& dimensions);

        int GetRenderWidth() const { return m_renderWidth; }
        int GetRenderHeight() const { return m_renderHeight; }
        int GetWindowWidth() const { return m_windowWidth; }
        int GetWindowHeight() const { return m_windowHeight; }

        void SetEventListener(EventListener* listener) { m_eventListener = listener; }
        void BroadcastEvent(Event const& event);

        LayerStack(LayerStack const&) = delete;
        LayerStack(LayerStack&&) = delete;
        LayerStack& operator=(LayerStack const&) = delete;
        LayerStack& operator=(LayerStack&&) = delete;
        ~LayerStack() override = default;

    private:
        IRenderer& m_renderer;
        std::vector<std::unique_ptr<Layer>> m_layers;
        EventListener* m_eventListener = nullptr;

        int m_renderWidth = 0;
        int m_renderHeight = 0;

        int m_windowWidth = 0;
        int m_windowHeight = 0;
    };
}
