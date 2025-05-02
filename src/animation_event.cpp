#include "common.h"
#include "moth_ui/animation_event.h"

namespace moth_ui {
    void to_json(nlohmann::json& j, AnimationEvent const& event) {
        j["frame"] = event.m_frame;
        j["name"] = event.m_name;
    }

    void from_json(nlohmann::json const& j, AnimationEvent& event) {
        j.at("frame").get_to(event.m_frame);
        j.at("name").get_to(event.m_name);
    }
}
