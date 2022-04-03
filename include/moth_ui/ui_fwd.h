#pragma once

namespace moth_ui {
    class IImage;
    class IFont;

    class Event;
    
    class Node;
    class Group;
    class NodeImage;

    class LayoutEntityText;
    class LayoutEntityImage;
    class LayoutEntityGroup;
    class LayoutEntity;

    class Keyframe;
    class AnimationEvent;
    class AnimationTrack;
    struct AnimationClip;

    class AnimationTrackController;
    class AnimationController;

    struct LayoutRect;

    using KeyframeValue = std::variant<float, std::string>;
}
