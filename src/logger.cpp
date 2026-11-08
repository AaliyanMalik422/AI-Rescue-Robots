#include "logger.hpp"
#include <iostream>
#include <fstream>

DynamicArray<std::string> Logger::entries;

void Logger::log(const std::string& message) {
    std::cout << "[LOG] " << message << '\n';
    entries.push_back(message);
}

bool Logger::save_to_file(const std::string& filename) {
    std::ofstream output(filename.c_str());
    if (!output.is_open()) {
        std::cerr << "Could not open log file: " << filename << '\n';
        return false;
    }

    for (int i = 0; i < entries.size(); ++i) {
        output << entries[i] << '\n';
    }

    output.close();
    return !output.fail();
}

void Logger::clear() {
    entries.clear();
}
