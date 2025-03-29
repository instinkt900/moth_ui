#pragma once

#include "node.h"

namespace moth_ui {
    class NodeRect : public Node {
    public:
        NodeRect(Context& context);
        NodeRect(Context& context, std::shared_ptr<LayoutEntityRect> layoutEntity);
        virtual ~NodeRect();

    protected:
        bool m_filled = true;

        void ReloadEntityInternal() override;
        void DrawInternal() override;

    private:
        void ReloadEntityPrivate();
    };
}
