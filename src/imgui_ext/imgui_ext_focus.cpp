#include "common.h"
#include "moth_ui/utils/imgui_ext_focus.h"

namespace {
    struct FocusContextDetails {
        imgui_ext::FocusGroupContext* ExistingContext;
        std::string NewFocusLabel;
        std::map<std::string, std::function<void()>> LostFocusCallbacks;
    };

    FocusContextDetails g_focusContext;
}

namespace imgui_ext {
    using namespace moth_ui;

    void FocusGroupBegin(FocusGroupContext* context) {
        g_focusContext.ExistingContext = context;
        g_focusContext.NewFocusLabel.clear();
        g_focusContext.LostFocusCallbacks.clear();
    }

    void FocusGroupInputText(char const* label, std::string const& target, std::function<void(std::string const&)> const& onChanged, std::function<void()> const& onLostFocus) {
        static char buffer[1024];
        strncpy(buffer, target.c_str(), 1024);

        g_focusContext.LostFocusCallbacks[label] = onLostFocus;
        if (ImGui::InputText(label, buffer, 1024)) {
            onChanged(buffer);
        }

        if (ImGui::IsItemFocused()) {
            g_focusContext.NewFocusLabel = label;
        }
    }

    void FocusGroupInputInt(char const* label, int value, std::function<void(int)> const& onChanged, std::function<void()> const& onLostFocus) {
        g_focusContext.LostFocusCallbacks[label] = onLostFocus;

        if (ImGui::InputInt(label, &value, 0)) {
            onChanged(value);
        }

        if (ImGui::IsItemFocused()) {
            g_focusContext.NewFocusLabel = label;
        }
    }

    void FocusGroupInputFloat(char const* label, float value, std::function<void(float)> const& onChanged, std::function<void()> const& onLostFocus) {
        g_focusContext.LostFocusCallbacks[label] = onLostFocus;

        if (ImGui::InputFloat(label, &value, 0)) {
            onChanged(value);
        }

        if (ImGui::IsItemFocused()) {
            g_focusContext.NewFocusLabel = label;
        }
    }

    void FocusGroupInputLayoutRect(char const* label, LayoutRect value, std::function<void(LayoutRect const&)> const& onChanged, std::function<void()> const& onLostFocus) {
        g_focusContext.LostFocusCallbacks[label] = onLostFocus;

        bool changed = false;
        if (ImGui::CollapsingHeader(label)) {
            if (ImGui::TreeNode("Anchor")) {
                ImGui::PushItemWidth(50);
                changed |= ImGui::InputFloat("Top", &value.anchor.topLeft.y, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                changed |= ImGui::InputFloat("Left", &value.anchor.topLeft.x, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                changed |= ImGui::InputFloat("Bottom", &value.anchor.bottomRight.y, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                changed |= ImGui::InputFloat("Right", &value.anchor.bottomRight.x, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                ImGui::PopItemWidth();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Offset")) {
                ImGui::PushItemWidth(50);
                changed |= ImGui::InputFloat("Top", &value.offset.topLeft.y, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                changed |= ImGui::InputFloat("Left", &value.offset.topLeft.x, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                changed |= ImGui::InputFloat("Bottom", &value.offset.bottomRight.y, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                changed |= ImGui::InputFloat("Right", &value.offset.bottomRight.x, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                ImGui::PopItemWidth();
                ImGui::TreePop();
            }
        }

        if (changed) {
            onChanged(value);
        }
    }

    void FocusGroupEnd() {
        if (!g_focusContext.ExistingContext->currentFocusLabel.empty() && g_focusContext.ExistingContext->currentFocusLabel != g_focusContext.NewFocusLabel) {
            auto const it = g_focusContext.LostFocusCallbacks.find(g_focusContext.ExistingContext->currentFocusLabel);
            if (std::end(g_focusContext.LostFocusCallbacks) != it) {
                it->second();
            }
        }
        g_focusContext.ExistingContext->currentFocusLabel = g_focusContext.NewFocusLabel;
        g_focusContext.LostFocusCallbacks.clear();
    }
}
