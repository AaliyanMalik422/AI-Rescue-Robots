#include "../include/logger.hpp"
#include <iostream>
#include <fstream>

DynamicArray<std::string> Logger::log_entries;

void Logger::log(const std::string& message) {
    std::cout << "[LOG] " << message << std::endl;
    log_entries.push_back(message);
}

void Logger::save_to_file(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (size_t i = 0; i < log_entries.size(); ++i) {
            file << log_entries[i] << "\n";
        }
        file.close();
    } else {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
    }
}

void Logger::clear() {
    log_entries.clear();
}
