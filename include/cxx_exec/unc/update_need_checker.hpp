#pragma once

#include <functional>

using update_need_checker = std::function<bool()>;

inline update_need_checker always = [](){ return true; };
inline update_need_checker never = [](){ return false; };