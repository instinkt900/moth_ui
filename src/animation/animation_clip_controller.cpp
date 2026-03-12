#include "common.h"
#include "moth_ui/animation/animation_clip_controller.h"
#include "moth_ui/animation/animation_controller.h"
#include "moth_ui/animation/animation_clip.h"
#include "moth_ui/animation/animation_event.h"
#include "moth_ui/events/event_animation.h"
#include "moth_ui/nodes/group.h"
#include "moth_ui/layout/layout_entity_group.h"

namespace moth_ui {
    AnimationClipController::AnimationClipController(Group* group)
        : m_group(group) {
    }

    void AnimationClipController::SetClip(std::shared_ptr<AnimationClip> clip) {
        m_clip = clip;
        if (clip) {
            m_frame = static_cast<float>(clip->m_startFrame);
            for (auto& child : m_group->GetChildren()) {
                child->GetAnimationController().SetFrame(m_frame);
            }
            m_group->SendEvent(EventAnimationStarted(m_group, clip->m_name), Node::EventDirection::Up);
        }
    }

    void AnimationClipController::Update(float deltaSeconds) {
        if (auto const clip = m_clip.lock()) {
            struct Span {
                float Start = 0;
                float End = 0;
                bool IsSet = false;
                void Set(float start, float end) {
                    Start = start;
                    End = end;
                    IsSet = true;
                }
            };

            std::array<Span, 2> eventChecks;

            auto const oldFrame = m_frame;
            auto const deltaFrames = deltaSeconds * clip->m_fps;
            m_frame += deltaFrames;

            bool animationEnded = false;
            std::string const animationName = clip->m_name;
            if (m_frame >= static_cast<float>(clip->m_endFrame)) {
                switch (clip->m_loopType) {
                case AnimationClip::LoopType::Stop:
                    m_frame = static_cast<float>(clip->m_endFrame);
                    m_clip.reset();
                    eventChecks[0].Set(oldFrame, m_frame);
                    animationEnded = true;
                    break;
                case AnimationClip::LoopType::Loop:
                    eventChecks[0].Set(oldFrame, static_cast<float>(clip->m_endFrame));
                    m_frame -= static_cast<float>(clip->m_endFrame - clip->m_startFrame);
                    eventChecks[1].Set(static_cast<float>(clip->m_startFrame), m_frame);
                    break;
                case AnimationClip::LoopType::Reset:
                    eventChecks[0].Set(oldFrame, static_cast<float>(clip->m_endFrame));
                    m_frame = static_cast<float>(clip->m_startFrame);
                    m_clip.reset();
                    animationEnded = true;
                    break;
                }
            } else {
                eventChecks[0].Set(oldFrame, m_frame);
            }

            // update each tracks time
            for (auto& child : m_group->GetChildren()) {
                child->GetAnimationController().SetFrame(m_frame);
            }

            // check for events over spans
            for (auto&& check : eventChecks) {
                if (check.IsSet) {
                    CheckEvents(check.Start, check.End);
                }
            }

            if (animationEnded) {
                m_group->SendEvent(EventAnimationStopped(m_group, animationName), Node::EventDirection::Up);
            }
        }
    }

    void AnimationClipController::CheckEvents(float startFrame, float endFrame) {
        auto layout = std::static_pointer_cast<LayoutEntityGroup>(m_group->GetLayoutEntity());
        for (auto& animEvent : layout->m_events) {
            if (static_cast<float>(animEvent->m_frame) > startFrame && static_cast<float>(animEvent->m_frame) <= endFrame) {
                m_group->SendEvent(EventAnimation(m_group, animEvent->m_name), Node::EventDirection::Up);
            }
        }
    }
}
