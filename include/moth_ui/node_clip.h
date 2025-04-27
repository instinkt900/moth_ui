#pragma once

#include "moth_ui/moth_ui.h"
#include "moth_ui/node.h"

#include <memory>

namespace moth_ui {
    class MOTH_UI_API NodeClip : public Node {
    public:
        NodeClip(Context& context);
        NodeClip(Context& context, std::shared_ptr<LayoutEntityClip> layoutEntity);
        virtual ~NodeClip();
    };
}
