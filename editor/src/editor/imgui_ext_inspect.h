#pragma once

#include "imgui_ext.h"
#include "moth_ui/layout/layout_rect.h"
#include "moth_ui/animation_track.h"
#include "imgui_ext.h"
#include "image.h"

namespace imgui_ext {
    inline void Inspect(char const* name, bool& value) {
        ImGui::Checkbox(name, &value);
    }

    inline void Inspect(char const* name, int& value) {
        ImGui::InputInt(name, &value);
    }

    inline void Inspect(char const* name, int const& value) {
        ImGui::LabelText(name, "%d", value);
    }

    inline void Inspect(char const* name, float& value) {
        ImGui::InputFloat(name, &value);
    }

    inline void Inspect(char const* name, char const* value) {
        ImGui::LabelText(name, "%s", value);
    }

    inline void Inspect(char const* name, std::string const& value) {
        ImGui::LabelText(name, "%s", value.c_str());
    }

    template <typename T>
    inline void Inspect(char const* name, std::vector<T>& value) {
        if (ImGui::CollapsingHeader(name)) {
            for (int i = 0; i < value.size(); ++i) {
                auto const label = fmt::format("{}", i);
                Inspect(label.c_str(), value[i]);
            }
        }
    }

    template <typename T>
    inline void Inspect(char const* name, std::function<T> const& value) {
        ImGui::LabelText(name, "<function>");
    }

    template <typename T>
    inline void Inspect(char const* name, T* value) {
        if (nullptr == value) {
            ImGui::LabelText(name, "nullptr");
        } else {
            Inspect(name, *value);
        }
    }

    template <typename T, std::enable_if_t<std::is_enum_v<T>, bool> = true>
    inline bool Inspect(char const* label, T& value) {
        bool changed = false;
        std::string const enumValueStr(magic_enum::enum_name(value));
        if (ImGui::BeginCombo(label, enumValueStr.c_str())) {
            for (size_t i = 0; i < magic_enum::enum_count<T>(); ++i) {
                auto const currentEnumValue = magic_enum::enum_value<T>(i);
                bool selected = currentEnumValue == value;
                std::string const currentValueStr(magic_enum::enum_name(currentEnumValue));
                if (ImGui::Selectable(currentValueStr.c_str(), selected)) {
                    value = currentEnumValue;
                    changed = true;
                }
            }
            ImGui::EndCombo();
        }
        return changed;
    }

    inline bool Inspect(char const* label, moth_ui::LayoutRect& widgetBounds) {
        bool changed = false;
        ImGui::PushID(label);
        if (ImGui::CollapsingHeader(label)) {
            if (ImGui::TreeNode("Anchor")) {
                ImGui::PushItemWidth(50);
                changed |= ImGui::InputFloat("Top", &widgetBounds.anchor.topLeft.y, 0, 0, "%.2f");
                changed |= ImGui::InputFloat("Left", &widgetBounds.anchor.topLeft.x, 0, 0, "%.2f");
                changed |= ImGui::InputFloat("Bottom", &widgetBounds.anchor.bottomRight.y, 0, 0, "%.2f");
                changed |= ImGui::InputFloat("Right", &widgetBounds.anchor.bottomRight.x, 0, 0, "%.2f");
                ImGui::PopItemWidth();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Offset")) {
                ImGui::PushItemWidth(50);
                changed |= ImGui::InputFloat("Top", &widgetBounds.offset.topLeft.y, 0, 0, "%.2f");
                changed |= ImGui::InputFloat("Left", &widgetBounds.offset.topLeft.x, 0, 0, "%.2f");
                changed |= ImGui::InputFloat("Bottom", &widgetBounds.offset.bottomRight.y, 0, 0, "%.2f");
                changed |= ImGui::InputFloat("Right", &widgetBounds.offset.bottomRight.x, 0, 0, "%.2f");
                ImGui::PopItemWidth();
                ImGui::TreePop();
            }
        }
        ImGui::PopID();
        return changed;
    }

    inline bool Inspect(char const* label, moth_ui::IntRect& widgetRect) {
        bool changed = false;
        ImGui::PushID(label);
        if (ImGui::CollapsingHeader(label)) {
            ImGui::PushItemWidth(108);
            changed |= ImGui::InputInt("Top", &widgetRect.topLeft.y, 0);
            changed |= ImGui::InputInt("Left", &widgetRect.topLeft.x, 0);
            changed |= ImGui::InputInt("Bottom", &widgetRect.bottomRight.y, 0);
            changed |= ImGui::InputInt("Right", &widgetRect.bottomRight.x, 0);
            ImGui::PopItemWidth();
        }
        ImGui::PopID();
        return changed;
    }

    inline void Inspect(char const* label, moth_ui::Color& color) {
        ImGui::ColorEdit4(label, reinterpret_cast<float*>(&color));
    }

    inline bool Inspect(char const* label, moth_ui::KeyframeValue& value) {
        if (value.index() == 0) {
            return ImGui::InputFloat(label, &std::get<float>(value));
        } else {
            return imgui_ext::InputString(label, &std::get<std::string>(value));
        }
    }

    inline void Inspect(char const* label, moth_ui::Keyframe& keyframe) {
        ImGui::PushID(label);
        if (ImGui::CollapsingHeader(label)) {
            Inspect("Frame", keyframe.m_frame);
            Inspect("Value", keyframe.m_value);
            Inspect("Interp", keyframe.m_interpType);
        }
        ImGui::PopID();
    }

    inline void Inspect(char const* name, moth_ui::IImage const* value) {
        if (value == nullptr) {
            return;
        }
        auto const image = static_cast<Image const*>(value);
        auto const texture = image->GetTexture();
        auto const& sourceRect = image->GetSourceRect();
        auto const textureDimensions = static_cast<moth_ui::FloatVec2>(image->GetTextureDimensions());
        auto const uv0 = static_cast<moth_ui::FloatVec2>(sourceRect.topLeft) / textureDimensions;
        auto const uv1 = static_cast<moth_ui::FloatVec2>(sourceRect.bottomRight) / textureDimensions;

        float constexpr ImageWidth = 200;
        auto const imageDimensions = image->GetDimensions();
        auto const scale = ImageWidth / imageDimensions.x;
        auto const scaledDimensions = static_cast<moth_ui::FloatVec2>(imageDimensions) * scale;
        ImGui::Image(texture.get(), ImVec2(scaledDimensions.x, scaledDimensions.y), ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y));
    }
}
