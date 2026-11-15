#pragma once

#include <string>
#include "types.hpp"

// =====================================================================
// PROVIDED (already written - do not spend time on these).
// Name <-> code tables and two small helpers shared by everyone.
//   * name_of functions turn a constant into text for logs and reports.
//   * parse_ functions turn a file word into a constant; they return 0 for an
//     unknown word, so the parser can reject it.
// =====================================================================

const char* location_type_name(int type);
const char* capability_name(int capability);
const char* category_name(int category);
const char* request_state_name(int state);
const char* robot_state_name(int state);
const char* event_type_name(int type);

int parse_location_type(const std::string& word);   // JUNCTION, BUILDING, ... -> LOCATION_*
int parse_capability(const std::string& word);      // MEDICAL, ..., GENERAL   -> CAPABILITY_*
int parse_category(const std::string& word);        // MEDICAL, FIRE, SUPPLY, SEARCH -> CATEGORY_*
int parse_event_type(const std::string& word);      // BLOCK, UNBLOCK, ...     -> EVENT_*

std::string int_to_string(long long value);
// Whole-token integer parse: "12" -> true, "12x" / "" / "1.5" -> false.
bool parse_int(const std::string& token, int& value);
