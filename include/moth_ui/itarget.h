#pragma once

#include "moth_ui/moth_ui.h"
#include "moth_ui/ui_fwd.h"
#include "moth_ui/utils/vector.h"

namespace moth_ui {
    class MOTH_UI_API ITarget {
    public:
        virtual ~ITarget() = default;

        virtual moth_ui::IntVec2 GetDimensions() const = 0;
        virtual IImage* GetImage() = 0;
    };
}
