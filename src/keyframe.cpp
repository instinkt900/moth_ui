#include "common.h"
#include "moth_ui/keyframe.h"

namespace nlohmann {
    template <>
    struct adl_serializer<std::variant<float, std::string>> {
        static void from_json(json const& j, std::variant<float, std::string>& var) {
            size_t const index = j["index"];
            if (index == 0) {
                var = static_cast<float>(j["value"]);
            } else if (index == 1) {
                var = static_cast<std::string>(j["value"]);
            }
        }

        static void to_json(json& j, std::variant<float, std::string> const& var) {
            j["index"] = var.index();
            std::visit([&](auto&& value) {
                j["value"] = value;
            },
                       var);
        }
    };
}

namespace moth_ui {
    void to_json(nlohmann::json& j, Keyframe const& keyframe) {
        j["frame"] = keyframe.m_frame;
        j["value"] = keyframe.m_value;
        j["interp"] = keyframe.m_interpType;
    }

    void from_json(nlohmann::json const& j, Keyframe& keyframe) {
        nlohmann::json valueJson = j.at("value");
        if (valueJson.is_number_float()) {
            valueJson.get_to(keyframe.m_value);
        } else {
            std::variant<float, std::string> oldValueType;
            valueJson.get_to(oldValueType);
            keyframe.m_value = std::get<float>(oldValueType);
        }

        j.at("frame").get_to(keyframe.m_frame);
        j.at("interp").get_to(keyframe.m_interpType);
    }
}
