#pragma once

#include "update_need_checker.hpp"
#include <filesystem>

inline update_need_checker by_date(std::filesystem::path src, std::filesystem::path out) {
    return [=]() {
        return
            not std::filesystem::exists(out)
            or std::filesystem::last_write_time(src) > std::filesystem::last_write_time(out);
    };
};