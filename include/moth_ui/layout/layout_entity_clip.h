#pragma once

#include "moth_ui/moth_ui.h"
#include "moth_ui/layout/layout_entity.h"

namespace moth_ui {
    class MOTH_UI_API LayoutEntityClip : public LayoutEntity {
    public:
        explicit LayoutEntityClip(LayoutRect const& initialBounds);
        explicit LayoutEntityClip(LayoutEntityGroup* parent);

        std::shared_ptr<LayoutEntity> Clone(CloneType cloneType) override;

        LayoutEntityType GetType() const override { return LayoutEntityType::Clip; }

        std::unique_ptr<Node> Instantiate(Context& context) override;
    };
}
