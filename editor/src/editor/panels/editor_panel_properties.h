#pragma once

#include "editor_panel.h"
#include "moth_ui/context.h"

class EditorPanelProperties : public EditorPanel {
public:
    EditorPanelProperties(EditorLayer& editorLayer, bool visible);
    virtual ~EditorPanelProperties() = default;

    void OnLayoutLoaded() override;

private:
    bool BeginPanel() override;
    void EndPanel() override;
    void DrawContents() override;

    std::shared_ptr<moth_ui::Node> m_currentSelection = nullptr;
    std::shared_ptr<moth_ui::Node> m_lastSelection = nullptr;

    void DrawNodeProperties(std::shared_ptr<moth_ui::Node> node, bool recurseChildren = true);
    void DrawCommonProperties(std::shared_ptr<moth_ui::Node> node);
    void DrawRectProperties(std::shared_ptr<moth_ui::NodeRect> node);
    void DrawImageProperties(std::shared_ptr<moth_ui::NodeImage> node);
    void DrawTextProperties(std::shared_ptr<moth_ui::NodeText> node);
    void DrawRefProperties(std::shared_ptr<moth_ui::Group> node, bool recurseChildren);
    void DrawLayoutProperties(std::shared_ptr<moth_ui::Group> node);
};
