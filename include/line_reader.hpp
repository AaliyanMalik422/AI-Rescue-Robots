#pragma once

#include <fstream>
#include <string>

// =====================================================================
// PROVIDED (already written) - file-reading helpers for the scenario parser.
// =====================================================================

// Reads the next line that is not blank and does not start with '#'.
// Returns false at the end of the file.
bool read_content_line(std::ifstream& file, std::string& line);

// Splits a line into words. Fills tokens[0..max_tokens-1] and returns the TOTAL number
// of words on the line, so a result larger than max_tokens means "too many words".
int split_tokens(const std::string& line, std::string tokens[], int max_tokens);
