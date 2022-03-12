#include "common.h"
#include "properties_editor.h"
#include "editor_layer.h"
#include "moth_ui/layout/layout_entity_image.h"
#include "moth_ui/utils/imgui_ext_inspect.h"
#include "moth_ui/node_image.h"
#include "utils.h"

namespace {
    ImGui::FileBrowser s_fileBrowser;
    std::shared_ptr<moth_ui::NodeImage> s_loadingNodeImage = nullptr;
}

PropertiesEditor::PropertiesEditor(EditorLayer& editorLayer)
    : m_editorLayer(editorLayer) {
}

PropertiesEditor::~PropertiesEditor() {
}

void PropertiesEditor::Draw() {
    if (auto selection = m_editorLayer.GetSelection()) {
        auto entity = selection->GetLayoutEntity();
        imgui_ext::FocusGroupBegin(&m_focusContext);
        DrawEntityProperties();
        switch (entity->GetType()) {
        case moth_ui::LayoutEntityType::Entity:
            break;
        case moth_ui::LayoutEntityType::Group:
            DrawGroupProperties();
            break;
        case moth_ui::LayoutEntityType::Image:
            DrawImageProperties();
            break;
        }
        imgui_ext::FocusGroupEnd();
    }
}

void PropertiesEditor::DrawEntityProperties() {
    int const frame = m_editorLayer.GetSelectedFrame();
    auto selection = m_editorLayer.GetSelection();
    auto entity = selection->GetLayoutEntity();

    imgui_ext::FocusGroupInputText(
        "ID", entity->GetId(),
        [&](std::string const& value) { entity->SetId(value); },
        [&]() {});

    bool visible = selection->IsVisible();
    ImGui::Checkbox("Visible", &visible);
    if (visible != selection->IsVisible()) {
        selection->SetVisible(visible);
    }

    bool showRect = selection->GetShowRect();
    ImGui::Checkbox("Show Rect", &showRect);
    if (showRect != selection->GetShowRect()) {
        selection->SetShowRect(showRect);
    }

    imgui_ext::FocusGroupInputLayoutRect(
        "Bounds", selection->GetLayoutRect(),
        [&](moth_ui::LayoutRect const& value) { m_editorLayer.BeginEditBounds(); selection->GetLayoutRect() = value; selection->RecalculateBounds(); },
        [&]() { m_editorLayer.EndEditBounds(); });

    imgui_ext::FocusGroupInputColor(
        "Color", selection->GetColor(),
        [&](moth_ui::Color const& value) { m_editorLayer.BeginEditColor(); selection->SetColor(value); },
        [&]() { m_editorLayer.EndEditColor(); });

    std::string blendModeStr(magic_enum::enum_name(selection->GetBlendMode()));
    if (ImGui::BeginCombo("Blend Mode", blendModeStr.c_str())) {
        for (int i = 0; i < magic_enum::enum_count<moth_ui::BlendMode>(); ++i) {
            auto const mode = magic_enum::enum_value<moth_ui::BlendMode>(i);
            bool selected = mode == selection->GetBlendMode();
            std::string str(magic_enum::enum_name(mode));
            if (ImGui::Selectable(str.c_str(), selected)) {
                selection->SetBlendMode(mode);
                entity->SetBlendMode(mode);
            }
        }
        ImGui::EndCombo();
    }
}

void PropertiesEditor::DrawGroupProperties() {
}

void PropertiesEditor::DrawImageProperties() {
    auto const selection = m_editorLayer.GetSelection();
    auto const imageNode = std::static_pointer_cast<moth_ui::NodeImage>(selection);
    auto const imageEntity = std::static_pointer_cast<moth_ui::LayoutEntityImage>(selection->GetLayoutEntity());
    // do we want to allow the animation of the source rect? would be nice
    imgui_ext::FocusGroupInputRect(
        "Source Rect", imageEntity->m_sourceRect,
        [&](moth_ui::IntRect const& value) {
            /*m_editorLayer.BeginEditBounds()*/;
            imageEntity->m_sourceRect = value;
            selection->ReloadEntity();
        },
        [&]() {
            /*m_editorLayer.EndEditBounds();*/
        });


    imgui_ext::Inspect("Image", imageNode->GetImage());

    if (ImGui::Button("Load Image..")) {
        s_fileBrowser.SetTitle("Load Image..");
        s_fileBrowser.SetTypeFilters({ ".jpg", ".jpeg", ".png", ".bmp" });
        s_fileBrowser.Open();
        s_loadingNodeImage = imageNode;
    }

    if (s_loadingNodeImage) {
        s_fileBrowser.Display();
        if (s_fileBrowser.HasSelected()) {
            auto const targetImageEntity = std::static_pointer_cast<moth_ui::LayoutEntityImage>(s_loadingNodeImage->GetLayoutEntity());
            targetImageEntity->m_texturePath = s_fileBrowser.GetSelected().string();
            s_loadingNodeImage->ReloadEntity();
            s_fileBrowser.ClearSelected();
            s_loadingNodeImage = nullptr;
        }
    }
}
