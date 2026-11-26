#include "logger.hpp"
#include "names.hpp"
#include <iostream>
#include <fstream>

Logger::Logger() : echo_to_console(false) {}

void Logger::log(int time, const std::string& event,
                 const std::string& ids, const std::string& detail) {

    std::string line = int_to_string(time) + "," + event + "," + ids + "," + detail;
    entries.push_back(line);

    if (echo_to_console)
        std::cout << line << '\n';
}

bool Logger::save_to_file(const std::string& filename) const {

    std::ofstream out(filename.c_str());

    if (!out.is_open())
        return false;

    out << "time,event,ids,detail\n";

    for (int i = 0; i < entries.size(); ++i)
        out << entries[i] << '\n';

    out.flush();
    return !out.fail();
}
