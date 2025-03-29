#pragma once

#include "node.h"

namespace moth_ui {
    class NodeClip : public Node {
    public:
        NodeClip(Context& context);
        NodeClip(Context& context, std::shared_ptr<LayoutEntityClip> layoutEntity);
        virtual ~NodeClip();
    };
}
