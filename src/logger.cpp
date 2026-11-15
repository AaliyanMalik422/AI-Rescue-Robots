#include "logger.hpp"
#include "names.hpp"
#include <iostream>
#include <fstream>

// MEMBER 2: logger implementation.

Logger::Logger() : echo_to_console(false) {}

void Logger::log(int time, const std::string& event,
                 const std::string& ids, const std::string& detail) {
    // DONE (provided)
    std::string line = int_to_string(time) + "," + event + "," + ids + "," + detail;
    entries.push_back(line);
    if (echo_to_console) std::cout << line << '\n';
}

bool Logger::save_to_file(const std::string& filename) const {
    // TODO (Member 2)
    // PSEUDOCODE: open an ofstream; if it failed to open: return false
    //   write the header line "time,event,ids,detail"
    //   for i in 0..entries.size()-1: write entries[i] and a newline
    //   return true only if no write error occurred
    (void)filename;
    return false;
}
