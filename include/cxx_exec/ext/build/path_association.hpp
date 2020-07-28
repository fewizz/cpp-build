#pragma once

#include <functional>
#include <filesystem>

using path_association = std::function<std::filesystem::path(std::filesystem::path)>;