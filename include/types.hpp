#pragma once

#include <string>

// Named integer constants are used instead of enums because the team has not
// studied enumerations yet. Keep these values consistent across all modules.

// Location types from Appendix A.
const int LOCATION_JUNCTION = 1;
const int LOCATION_BUILDING = 2;
const int LOCATION_HOSPITAL = 3;
const int LOCATION_DEPOT = 4;
const int LOCATION_CHARGING = 5;
const int LOCATION_COMMAND = 6;

// Robot capabilities and rescue request categories.
const int CAPABILITY_MEDICAL = 1;
const int CAPABILITY_FIRE = 2;
const int CAPABILITY_SUPPLY = 3;
const int CAPABILITY_SEARCH = 4;
const int CAPABILITY_GENERAL = 5;

const int CATEGORY_MEDICAL = 1;
const int CATEGORY_FIRE = 2;
const int CATEGORY_SUPPLY = 3;
const int CATEGORY_SEARCH = 4;

// Rescue request lifecycle.
const int REQUEST_PENDING = 1;
const int REQUEST_ASSIGNED = 2;
const int REQUEST_IN_PROGRESS = 3;
const int REQUEST_COMPLETED = 4;
const int REQUEST_FAILED = 5;

// Robot operational states.
const int ROBOT_IDLE = 1;
const int ROBOT_MOVING = 2;
const int ROBOT_RESCUING = 3;
const int ROBOT_RETURNING = 4;
const int ROBOT_CHARGING = 5;
const int ROBOT_OFFLINE = 6;

// Dynamic event types from Appendix A.
const int EVENT_BLOCK = 1;
const int EVENT_UNBLOCK = 2;
const int EVENT_STATION_DOWN = 3;
const int EVENT_STATION_UP = 4;
const int EVENT_SET_URGENCY = 5;
const int EVENT_ROBOT_OFFLINE = 6;
const int EVENT_ROBOT_ONLINE = 7;

struct Location {
    int id;
    std::string name;
    int type;
    bool station_available;

    Location()
        : id(-1), name(""), type(LOCATION_JUNCTION), station_available(false) {}

    Location(int location_id, const std::string& location_name, int location_type)
        : id(location_id), name(location_name), type(location_type),
          station_available(location_type == LOCATION_CHARGING) {}
};

struct Connection {
    int id;
    int endpoint1_id;
    int endpoint2_id;
    int distance;
    bool is_blocked;

    Connection()
        : id(-1), endpoint1_id(-1), endpoint2_id(-1), distance(0), is_blocked(false) {}

    Connection(int connection_id, int first_endpoint, int second_endpoint,
               int connection_distance, bool blocked)
        : id(connection_id), endpoint1_id(first_endpoint), endpoint2_id(second_endpoint),
          distance(connection_distance), is_blocked(blocked) {}
};
