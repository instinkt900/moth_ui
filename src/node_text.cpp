#include "common.h"
#include "moth_ui/node_text.h"
#include "moth_ui/layout/layout_entity.h"
#include "moth_ui/layout/layout_entity_text.h"
#include "moth_ui/utils/imgui_ext_inspect.h"
#include "moth_ui/context.h"

namespace moth_ui {
    NodeText::NodeText() {
    }

    NodeText::NodeText(std::shared_ptr<LayoutEntityText> layoutEntity)
        : Node(layoutEntity)
        , m_text(layoutEntity->m_text)
        , m_horizontalAlignment(layoutEntity->m_horizontalAlignment)
        , m_verticalAlignment(layoutEntity->m_verticalAlignment) {
        Load(layoutEntity->m_fontName.c_str(), layoutEntity->m_fontSize);
    }

    NodeText::~NodeText() {
    }

    void NodeText::Load(char const* fontName, int size) {
        if (fontName == nullptr || *fontName == 0) {
            m_font = Context::GetCurrentContext().GetFontFactory().GetDefaultFont(size);
        } else {
            m_font = Context::GetCurrentContext().GetFontFactory().GetFont(fontName, size);
        }
    }

    void NodeText::ReloadEntity() {
        Node::ReloadEntity();
        auto layoutEntity = std::static_pointer_cast<LayoutEntityText>(m_layout);
        m_text = layoutEntity->m_text;
        m_horizontalAlignment = layoutEntity->m_horizontalAlignment;
        m_verticalAlignment = layoutEntity->m_verticalAlignment;
        Load(layoutEntity->m_fontName.c_str(), layoutEntity->m_fontSize);
    }

    void NodeText::DebugDraw() {
        Node::DebugDraw();
        if (ImGui::TreeNode("NodeText")) {
            //imgui_ext::Inspect("font", m_font);
            imgui_ext::Inspect("text", m_text.c_str());
            //imgui_ext::Inspect("alignment", m_alignment);
            ImGui::TreePop();
        }
    }

    void NodeText::DrawInternal() {
        if (m_font) {
            Context::GetCurrentContext().GetRenderer().RenderText(m_text, *m_font, m_horizontalAlignment, m_verticalAlignment, m_screenRect);
        }
    }
}
