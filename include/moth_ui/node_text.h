#pragma once

#include "moth_ui/moth_ui.h"
#include "moth_ui/node.h"
#include "moth_ui/text_alignment.h"

#include <memory>
#include <string>

namespace moth_ui {
    class MOTH_UI_API NodeText : public Node {
    public:
        NodeText(Context& context);
        NodeText(Context& context, std::shared_ptr<LayoutEntityText> layoutEntity);
        virtual ~NodeText();

        void Load(char const* fontName, int size);

        IFont* GetFont() const {
            return m_font.get();
        }

        void SetText(std::string const& text) { m_text = text; }
        std::string const& GetText() const { return m_text; }

    protected:
        std::shared_ptr<IFont> m_font;
        std::string m_text;
        TextHorizAlignment m_horizontalAlignment;
        TextVertAlignment m_verticalAlignment;
        bool m_dropShadow;
        IntVec2 m_dropShadowOffset;
        Color m_dropShadowColor;

        void ReloadEntityInternal() override;
        void DrawInternal() override;

    private:
        void ReloadEntityPrivate();
    };
}
