#pragma once

#include <string>
#include "collections.hpp"

class Logger {
private:
    static DynamicArray<std::string> entries;

public:
    static void log(const std::string& message);
    static bool save_to_file(const std::string& filename);
    static void clear();
};
