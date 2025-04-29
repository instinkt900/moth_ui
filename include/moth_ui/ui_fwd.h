#pragma once

#include "moth_ui/moth_ui.h"

namespace moth_ui {
    class MOTH_UI_API IImage;
    class MOTH_UI_API ITarget;
    class MOTH_UI_API IFont;

    class MOTH_UI_API Event;
    
    class MOTH_UI_API Node;
    class MOTH_UI_API Group;
    class MOTH_UI_API NodeRect;
    class MOTH_UI_API NodeImage;
    class MOTH_UI_API NodeText;
    class MOTH_UI_API NodeClip;

    class MOTH_UI_API Layout;
    class MOTH_UI_API LayoutEntityText;
    class MOTH_UI_API LayoutEntityImage;
    class MOTH_UI_API LayoutEntityRect;
    class MOTH_UI_API LayoutEntityGroup;
    class MOTH_UI_API LayoutEntityRef;
    class MOTH_UI_API LayoutEntityClip;
    class MOTH_UI_API LayoutEntity;

    class MOTH_UI_API Keyframe;
    class MOTH_UI_API AnimationEvent;
    class MOTH_UI_API AnimationTrack;
    struct MOTH_UI_API AnimationClip;

    class MOTH_UI_API AnimationTrackController;
    class MOTH_UI_API AnimationController;

    struct MOTH_UI_API LayoutRect;

    using KeyframeValue = float;
}
