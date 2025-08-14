#pragma once

#include "moth_ui/utils/vector.h"
#include <nlohmann/json.hpp>

namespace moth_ui {
    template <typename T, int Dim>
    inline void to_json(nlohmann::json& j, Vector<T, Dim> const& vec) {
        j = nlohmann::json::array();
        for (int i = 0; i < Dim; ++i) {
            j.push_back(vec.data[i]);
        }
    }

    template <typename T, int Dim>
    inline void from_json(nlohmann::json const& j, Vector<T, Dim>& vec) {
        for (int i = 0; i < Dim; ++i) {
            j[i].get_to(vec.data[i]);
        }
    }
}
