#pragma once

#include "moth_ui/layout/layout_entity.h"
#include "moth_ui/text_alignment.h"

namespace moth_ui {
    class LayoutEntityText : public LayoutEntity {
    public:
        explicit LayoutEntityText(LayoutRect const& initialBounds);
        explicit LayoutEntityText(LayoutEntityGroup* parent);

        LayoutEntityType GetType() const override { return LayoutEntityType::Text; }

        std::unique_ptr<Node> Instantiate() override;

        nlohmann::json Serialize() const override;
        void Deserialize(nlohmann::json const& json, int dataVersion) override;

        std::string m_fontName;
        int m_fontSize = 16;
        std::string m_text = "Text Element";
        TextHorizAlignment m_horizontalAlignment = TextHorizAlignment::Left;
        TextVertAlignment m_verticalAlignment = TextVertAlignment::Top;
    };
}
