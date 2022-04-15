#include "common.h"
#include "properties_editor.h"
#include "editor_layer.h"
#include "moth_ui/layout/layout_entity_rect.h"
#include "moth_ui/layout/layout_entity_image.h"
#include "moth_ui/layout/layout_entity_text.h"
#include "moth_ui/utils/imgui_ext_inspect.h"
#include "moth_ui/node_rect.h"
#include "moth_ui/node_image.h"
#include "moth_ui/node_text.h"
#include "utils.h"
#include "moth_ui/context.h"
#include "properties_elements.h"
#include "actions/editor_action.h"

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
        case moth_ui::LayoutEntityType::Rect:
            DrawRectProperties();
            break;
        case moth_ui::LayoutEntityType::Image:
            DrawImageProperties();
            break;
        case moth_ui::LayoutEntityType::Text:
            DrawTextProperties();
            break;
        }
        imgui_ext::FocusGroupEnd();
    } else {
        PropertiesInputClear();
    }
}

void PropertiesEditor::DrawEntityProperties() {
    auto const selection = m_editorLayer.GetSelection();
    auto const entity = selection->GetLayoutEntity();

    PropertiesInput(
        "ID", entity->m_id.c_str(),
        [&](auto changedValue) {
            selection->SetId(changedValue);
        },
        [=](std::string oldValue, std::string newValue) {
            auto action = MakeChangeValueAction(entity->m_id, oldValue, newValue, [selection]() { selection->ReloadEntity(); });
            m_editorLayer.PerformEditAction(std::move(action));
        });

    PropertiesInput("Visible", selection->IsVisible(), [&](bool value) {
        auto action = MakeVisibilityAction(selection, value);
        m_editorLayer.PerformEditAction(std::move(action));
    });

    PropertiesInput("Show Bounds", selection->GetShowRect(), [&](bool value) {
        auto action = MakeShowBoundsAction(selection, value);
        m_editorLayer.PerformEditAction(std::move(action));
    });

    PropertiesInput(
        "Bounds", selection->GetLayoutRect(),
        [&](moth_ui::LayoutRect changedValue) {
            m_editorLayer.BeginEditBounds();
            selection->GetLayoutRect() = changedValue;
            selection->RecalculateBounds();
        },
        [=](moth_ui::LayoutRect oldValue, moth_ui::LayoutRect newValue) {
            m_editorLayer.EndEditBounds();
        });

    PropertiesInput(
        "Color", selection->GetColor(),
        [&](moth_ui::Color changedValue) {
            m_editorLayer.BeginEditColor();
            selection->SetColor(changedValue);
        },
        [=](moth_ui::Color oldValue, moth_ui::Color newValue) {
            m_editorLayer.EndEditColor();
        });

    PropertiesInput(
        "Blend Mode", selection->GetBlendMode(),
        [&](auto oldValue, auto newValue) {
            auto action = MakeChangeValueAction(entity->m_blend, oldValue, newValue, [selection]() { selection->ReloadEntity(); });
            m_editorLayer.PerformEditAction(std::move(action));
        });
}

void PropertiesEditor::DrawGroupProperties() {
}

void PropertiesEditor::DrawRectProperties() {
    auto const selection = m_editorLayer.GetSelection();
    auto const rectNode = std::static_pointer_cast<moth_ui::NodeRect>(selection);
    auto const rectEntity = std::static_pointer_cast<moth_ui::LayoutEntityRect>(selection->GetLayoutEntity());

    PropertiesInput(
        "Filled", rectEntity->m_filled,
        [&](auto const newValue) {
            auto const oldValue = rectEntity->m_filled;
            auto action = MakeChangeValueAction(rectEntity->m_filled, oldValue, newValue, [selection]() { selection->ReloadEntity(); });
            m_editorLayer.PerformEditAction(std::move(action));
        });
}

void PropertiesEditor::DrawImageProperties() {
    auto const selection = m_editorLayer.GetSelection();
    auto const imageNode = std::static_pointer_cast<moth_ui::NodeImage>(selection);
    auto const imageEntity = std::static_pointer_cast<moth_ui::LayoutEntityImage>(selection->GetLayoutEntity());
    
    // do we want to allow the animation of the source rect? would be nice
    PropertiesInput(
        "Source Rect", imageEntity->m_sourceRect,
        [&](auto const newValue) {
            auto const oldValue = imageEntity->m_sourceRect;
            auto action = MakeChangeValueAction(imageEntity->m_sourceRect, oldValue, newValue, [selection]() { selection->ReloadEntity(); });
            m_editorLayer.PerformEditAction(std::move(action));
        },
        nullptr);

    PropertiesInput(
        "Image Scale Type", imageEntity->m_imageScaleType,
        [&](auto oldValue, auto newValue) {
            auto action = MakeChangeValueAction(imageEntity->m_imageScaleType, oldValue, newValue, [selection]() { selection->ReloadEntity(); });
            m_editorLayer.PerformEditAction(std::move(action));
        });

    PropertiesInput(
        "Image Scale", imageEntity->m_imageScale,
        [&](float changedValue) {
            imageEntity->m_imageScale = changedValue;
            selection->ReloadEntity();
        },
        [=](float oldValue, float newValue) {
            auto action = MakeChangeValueAction(imageEntity->m_imageScale, oldValue, newValue, [selection]() { selection->ReloadEntity(); });
            m_editorLayer.PerformEditAction(std::move(action));
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
            auto const oldPath = targetImageEntity->m_texturePath;
            auto const newPath = s_fileBrowser.GetSelected().string();
            auto action = MakeChangeValueAction(imageEntity->m_texturePath, oldPath, newPath, [selection]() { selection->ReloadEntity(); });
            m_editorLayer.PerformEditAction(std::move(action));
            s_fileBrowser.ClearSelected();
            s_loadingNodeImage = nullptr;
        }
    }
}

void PropertiesEditor::DrawTextProperties() {
    auto const selection = m_editorLayer.GetSelection();
    auto const textNode = std::static_pointer_cast<moth_ui::NodeText>(selection);
    auto const textEntity = std::static_pointer_cast<moth_ui::LayoutEntityText>(selection->GetLayoutEntity());

    PropertiesInput(
        "Font Size", textEntity->m_fontSize,
        [&](int changedValue) {
            textEntity->m_fontSize = changedValue;
            textNode->ReloadEntity();
        },
        [=](int oldValue, int newValue) {
            auto action = MakeChangeValueAction(textEntity->m_fontSize, oldValue, newValue, [selection]() { selection->ReloadEntity(); });
            m_editorLayer.PerformEditAction(std::move(action));
        });

    auto const fontNames = moth_ui::Context::GetCurrentContext().GetFontFactory().GetFontNameList();
    PropertiesInput(
        "Font", fontNames, textEntity->m_fontName,
        [&](auto oldValue, auto newValue) {
            auto action = MakeChangeValueAction(textEntity->m_fontName, oldValue, newValue, [selection]() { selection->ReloadEntity(); });
            m_editorLayer.PerformEditAction(std::move(action));
        });

    PropertiesInput(
        "H Alignment", textEntity->m_horizontalAlignment,
        [&](auto oldValue, auto newValue) {
            auto action = MakeChangeValueAction(textEntity->m_horizontalAlignment, oldValue, newValue, [selection]() { selection->ReloadEntity(); });
            m_editorLayer.PerformEditAction(std::move(action));
        });

    PropertiesInput(
        "V Alignment", textEntity->m_verticalAlignment,
        [&](auto oldValue, auto newValue) {
            auto action = MakeChangeValueAction(textEntity->m_verticalAlignment, oldValue, newValue, [selection]() { selection->ReloadEntity(); });
            m_editorLayer.PerformEditAction(std::move(action));
        });

    PropertiesInput(
        "Text", textEntity->m_text.c_str(), 8,
        [&](char const* changedValue) {
            textNode->SetText(changedValue);
        },
        [=](std::string oldValue, std::string newValue) {
            auto action = MakeChangeValueAction(textEntity->m_text, oldValue, newValue, [selection]() { selection->ReloadEntity(); });
            m_editorLayer.PerformEditAction(std::move(action));
        });
}
