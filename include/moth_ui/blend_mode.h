#pragma once

#include "moth_ui/moth_ui.h"

namespace moth_ui {
    enum class MOTH_UI_API BlendMode {
        Invalid = -1,
        Replace,
        Alpha,
        Add,
        Multiply,
        Modulate
    };
}
