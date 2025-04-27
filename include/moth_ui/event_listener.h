#pragma once

#include "moth_ui/moth_ui.h"
#include "ui_fwd.h"

namespace moth_ui {
    class MOTH_UI_API EventListener {
    public:
        virtual bool OnEvent(Event const& event) = 0;
    };
}
