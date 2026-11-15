#include "line_reader.hpp"
#include <sstream>

// PROVIDED - see include/line_reader.hpp.

bool read_content_line(std::ifstream& file, std::string& line) {
    while (std::getline(file, line)) {
        int i = 0;
        int size = (int)line.size();
        while (i < size && (line[i] == ' ' || line[i] == '\t' || line[i] == '\r')) ++i;
        if (i == size) continue;          // blank line
        if (line[i] == '#') continue;     // comment
        return true;
    }
    return false;
}

int split_tokens(const std::string& line, std::string tokens[], int max_tokens) {
    std::istringstream in(line);
    std::string word;
    int count = 0;
    while (in >> word) {
        if (count < max_tokens) tokens[count] = word;
        ++count;
    }
    return count;
}
