#include "common.h"
#include "editor_application.h"
#include "editor/editor_layer.h"
#include "editor/editor_layer_stack.h"

#include "moth_ui/event_dispatch.h"

char const* const EditorApplication::IMGUI_FILE = "imgui.ini";
char const* const EditorApplication::PERSISTENCE_FILE = "editor.json";
EditorApplication* g_App;

EditorApplication::EditorApplication()
    : Application("Moth UI Tool") {
    m_imguiSettingsPath = (std::filesystem::current_path() / IMGUI_FILE).string();
    m_persistentFilePath = std::filesystem::current_path() / PERSISTENCE_FILE;
    std::ifstream persistenceFile(m_persistentFilePath.string());
    if (persistenceFile.is_open()) {
        try {
            persistenceFile >> m_persistentState;
        } catch (std::exception&) {
        }

        if (!m_persistentState.is_null()) {
            auto const oldPos = m_windowPos;
            auto const oldWidth = m_windowWidth;
            auto const oldHeight = m_windowHeight;
            m_windowPos = m_persistentState.value("window_pos", m_windowPos);
            m_windowWidth = m_persistentState.value("window_width", m_windowWidth);
            m_windowHeight = m_persistentState.value("window_height", m_windowHeight);
            m_windowMaximized = m_persistentState.value("window_maximized", m_windowMaximized);
            if (m_windowPos.x <= 0 || m_windowPos.y <= 0) {
                m_windowPos = oldPos;
            }
            if (m_windowWidth <= 0) {
                m_windowWidth = oldWidth;
            }
            if (m_windowHeight <= 0) {
                m_windowHeight = oldHeight;
            }
        }
    }

    g_App = this;
}

EditorApplication::~EditorApplication() {
    std::ofstream ofile(m_persistentFilePath.string());
    if (ofile.is_open()) {
        m_persistentState["current_path"] = std::filesystem::current_path().string();
        m_persistentState["window_pos"] = m_windowPos;
        m_persistentState["window_width"] = m_windowWidth;
        m_persistentState["window_height"] = m_windowHeight;
        m_persistentState["window_maximized"] = m_windowMaximized;
        ofile << m_persistentState;
    }

    g_App = nullptr;
}

void EditorApplication::SetupLayers() {
    if (m_persistentState.contains("current_path")) {
        std::string const currentPath = m_persistentState["current_path"];
        try {
            std::filesystem::current_path(currentPath);
        } catch (std::exception&) {
            // ...
        }
    }

    m_layerStack = std::make_unique<EditorLayerStack>(m_windowWidth, m_windowHeight, m_windowWidth, m_windowHeight);
    m_layerStack->SetEventListener(this);
    m_layerStack->PushLayer(std::make_unique<EditorLayer>());
}
