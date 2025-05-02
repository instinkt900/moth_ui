#include "common.h"
#include "moth_ui/animation_track.h"
#include "moth_ui/keyframe.h"

namespace moth_ui {
    void to_json(nlohmann::json& j, AnimationTrack const& track) {
        j["target"] = track.m_target;
        std::vector<Keyframe> keyframes;
        for (auto& keyframe : track.m_keyframes) {
            keyframes.push_back(*keyframe);
        }
        j["keyframes"] = keyframes;
    }

    void from_json(nlohmann::json const& j, AnimationTrack& track) {
        track.m_target = j.value("target", AnimationTrack::Target::Unknown);
        auto keyframes = j.value("keyframes", std::vector<Keyframe>{});
        for (auto& keyframe : keyframes) {
            track.m_keyframes.push_back(std::make_unique<Keyframe>(keyframe));
        }
    }

    AnimationTrack::AnimationTrack(AnimationTrack const& other)
        : m_target(other.m_target) {
        for (auto&& keyframe : other.m_keyframes) {
            m_keyframes.push_back(std::make_shared<Keyframe>(*keyframe));
        }
    }

    AnimationTrack::AnimationTrack(Target target)
        : m_target(target) {
    }

    AnimationTrack::AnimationTrack(Target target, float initialValue)
        : m_target(target) {
        m_keyframes.push_back(std::make_unique<Keyframe>(0, initialValue));
    }

    AnimationTrack::AnimationTrack(nlohmann::json const& json) {
        *this = json;
        SortKeyframes();
    }

    Keyframe* AnimationTrack::GetKeyframe(int frameNo) {
        auto keyframeIt = ranges::find_if(m_keyframes, [&](auto const& kf) { return kf->m_frame == frameNo; });
        if (std::end(m_keyframes) != keyframeIt) {
            // found an existing frame
            return keyframeIt->get();
        }
        return nullptr;
    }

    Keyframe& AnimationTrack::GetOrCreateKeyframe(int frameNo) {
        // find the frame or the first iterator after where it would be
        auto keyframeIt = ranges::find_if(m_keyframes, [&](auto const& kf) { return kf->m_frame >= frameNo; });
        if (std::end(m_keyframes) != keyframeIt && (*keyframeIt)->m_frame == frameNo) {
            // found an existing frame
            return *(*keyframeIt);
        }

        // didnt find the frame. keyframeIt will be one above the new one
        auto newFrameIt = m_keyframes.insert(keyframeIt, std::make_unique<Keyframe>(frameNo, KeyframeValue{}));
        return *(*newFrameIt);
    }

    void AnimationTrack::DeleteKeyframe(int frameNo) {
        auto keyframeIt = ranges::find_if(m_keyframes, [&](auto const& kf) { return kf->m_frame == frameNo; });
        if (std::end(m_keyframes) != keyframeIt) {
            m_keyframes.erase(keyframeIt);
        }
    }

    void AnimationTrack::DeleteKeyframe(Keyframe* frame) {
        auto keyframeIt = ranges::find_if(m_keyframes, [&](auto const& kf) { return kf.get() == frame; });
        if (std::end(m_keyframes) != keyframeIt) {
            m_keyframes.erase(keyframeIt);
        }
    }

    void AnimationTrack::ForKeyframesOverFrames(float startFrame, float endFrame, std::function<void(Keyframe const&)> const& callback) {
        for (auto& keyframe : m_keyframes) {
            if (keyframe->m_frame > startFrame && keyframe->m_frame <= endFrame) {
                callback(*keyframe);
            }
        }
    }

    float AnimationTrack::GetValueAtFrame(float frame) const {
        float value = 0;

        auto endKeyframeIt = std::end(m_keyframes);
        auto firstKeyframeIt = std::begin(m_keyframes);
        auto secondKeyframeIt = firstKeyframeIt;

        // find the possible two keyframes bounding the current time
        while (secondKeyframeIt != endKeyframeIt && (*secondKeyframeIt)->m_frame < frame) {
            firstKeyframeIt = secondKeyframeIt;
            ++secondKeyframeIt;
        }

        if (endKeyframeIt == firstKeyframeIt && endKeyframeIt == secondKeyframeIt) {
            // did not find any frames
        } else if (endKeyframeIt != firstKeyframeIt && endKeyframeIt != secondKeyframeIt) {
            // found a start and end keyframe
            float const deltaFrames = frame - (*firstKeyframeIt)->m_frame;
            float const totalFrames = static_cast<float>((*secondKeyframeIt)->m_frame - (*firstKeyframeIt)->m_frame);
            float const factor = totalFrames == 0.0f ? 0.0f : deltaFrames / totalFrames;
            float const startValue = (*firstKeyframeIt)->m_value;
            float const endValue = (*secondKeyframeIt)->m_value;
            value = Interp(startValue, endValue, factor, (*firstKeyframeIt)->m_interpType);
        } else {
            // found one keyframe
            auto validKeyframeIt = firstKeyframeIt != endKeyframeIt ? firstKeyframeIt : secondKeyframeIt;
            value = (*validKeyframeIt)->m_value;
        }

        return value;
    }

    void AnimationTrack::SortKeyframes() {
        ranges::sort(m_keyframes, [](auto const& a, auto const& b) {
            return a->m_frame < b->m_frame;
        });
    }
}
