#pragma once

#include "moth_ui/moth_ui.h"

namespace moth_ui {
    enum class MOTH_UI_API LayoutEntityType {
        Unknown,
        Layout,
        Group,
        Ref,
        Entity,
        Rect,
        Image,
        Text,
        Clip,
    };
}
