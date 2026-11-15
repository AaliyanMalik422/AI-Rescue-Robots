#include "names.hpp"
#include <sstream>

// PROVIDED - see include/names.hpp.

const char* location_type_name(int type) {
    if (type == LOCATION_JUNCTION) return "JUNCTION";
    if (type == LOCATION_BUILDING) return "BUILDING";
    if (type == LOCATION_HOSPITAL) return "HOSPITAL";
    if (type == LOCATION_DEPOT) return "DEPOT";
    if (type == LOCATION_CHARGING) return "CHARGING";
    if (type == LOCATION_COMMAND) return "COMMAND";
    return "UNKNOWN";
}

const char* capability_name(int capability) {
    if (capability == CAPABILITY_MEDICAL) return "MEDICAL";
    if (capability == CAPABILITY_FIRE) return "FIRE";
    if (capability == CAPABILITY_SUPPLY) return "SUPPLY";
    if (capability == CAPABILITY_SEARCH) return "SEARCH";
    if (capability == CAPABILITY_GENERAL) return "GENERAL";
    return "UNKNOWN";
}

const char* category_name(int category) {
    if (category == CATEGORY_MEDICAL) return "MEDICAL";
    if (category == CATEGORY_FIRE) return "FIRE";
    if (category == CATEGORY_SUPPLY) return "SUPPLY";
    if (category == CATEGORY_SEARCH) return "SEARCH";
    return "UNKNOWN";
}

const char* request_state_name(int state) {
    if (state == REQUEST_UNREPORTED) return "UNREPORTED";
    if (state == REQUEST_PENDING) return "PENDING";
    if (state == REQUEST_ASSIGNED) return "ASSIGNED";
    if (state == REQUEST_IN_PROGRESS) return "IN_PROGRESS";
    if (state == REQUEST_COMPLETED) return "COMPLETED";
    if (state == REQUEST_FAILED) return "FAILED";
    return "UNKNOWN";
}

const char* robot_state_name(int state) {
    if (state == ROBOT_AVAILABLE) return "AVAILABLE";
    if (state == ROBOT_TRAVELLING) return "TRAVELLING";
    if (state == ROBOT_PERFORMING) return "PERFORMING";
    if (state == ROBOT_RETURNING) return "RETURNING";
    if (state == ROBOT_RECHARGING) return "RECHARGING";
    if (state == ROBOT_OFFLINE) return "OFFLINE";
    if (state == ROBOT_UNAVAILABLE) return "UNAVAILABLE";
    return "UNKNOWN";
}

const char* event_type_name(int type) {
    if (type == EVENT_BLOCK) return "BLOCK";
    if (type == EVENT_UNBLOCK) return "UNBLOCK";
    if (type == EVENT_STATION_DOWN) return "STATION_DOWN";
    if (type == EVENT_STATION_UP) return "STATION_UP";
    if (type == EVENT_SET_URGENCY) return "SET_URGENCY";
    if (type == EVENT_ROBOT_OFFLINE) return "ROBOT_OFFLINE";
    if (type == EVENT_ROBOT_ONLINE) return "ROBOT_ONLINE";
    return "UNKNOWN";
}

int parse_location_type(const std::string& word) {
    if (word == "JUNCTION") return LOCATION_JUNCTION;
    if (word == "BUILDING") return LOCATION_BUILDING;
    if (word == "HOSPITAL") return LOCATION_HOSPITAL;
    if (word == "DEPOT") return LOCATION_DEPOT;
    if (word == "CHARGING") return LOCATION_CHARGING;
    if (word == "COMMAND") return LOCATION_COMMAND;
    return 0;
}

int parse_capability(const std::string& word) {
    if (word == "MEDICAL") return CAPABILITY_MEDICAL;
    if (word == "FIRE") return CAPABILITY_FIRE;
    if (word == "SUPPLY") return CAPABILITY_SUPPLY;
    if (word == "SEARCH") return CAPABILITY_SEARCH;
    if (word == "GENERAL") return CAPABILITY_GENERAL;
    return 0;
}

int parse_category(const std::string& word) {
    if (word == "MEDICAL") return CATEGORY_MEDICAL;
    if (word == "FIRE") return CATEGORY_FIRE;
    if (word == "SUPPLY") return CATEGORY_SUPPLY;
    if (word == "SEARCH") return CATEGORY_SEARCH;
    return 0;
}

int parse_event_type(const std::string& word) {
    if (word == "BLOCK") return EVENT_BLOCK;
    if (word == "UNBLOCK") return EVENT_UNBLOCK;
    if (word == "STATION_DOWN") return EVENT_STATION_DOWN;
    if (word == "STATION_UP") return EVENT_STATION_UP;
    if (word == "SET_URGENCY") return EVENT_SET_URGENCY;
    if (word == "ROBOT_OFFLINE") return EVENT_ROBOT_OFFLINE;
    if (word == "ROBOT_ONLINE") return EVENT_ROBOT_ONLINE;
    return 0;
}

std::string int_to_string(long long value) {
    std::ostringstream out;
    out << value;
    return out.str();
}

bool parse_int(const std::string& token, int& value) {
    if (token.empty()) return false;
    std::istringstream in(token);
    long long parsed = 0;
    in >> parsed;
    if (in.fail() || !in.eof()) return false;
    if (parsed < -2000000000LL || parsed > 2000000000LL) return false;
    value = (int)parsed;
    return true;
}
