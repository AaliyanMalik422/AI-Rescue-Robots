#pragma once

#include <string>

// =====================================================================
// SHARED FILE - owned by the whole team. Changing a constant or struct here
// affects every module, so tell the other two members before editing.
//
// Named integer constants are used instead of enums (enums not studied).
// State names follow the Phase 1 Design Report (sections 3.2 and 4.3).
// =====================================================================

// Location types (Appendix A).
const int LOCATION_JUNCTION = 1;
const int LOCATION_BUILDING = 2;
const int LOCATION_HOSPITAL = 3;
const int LOCATION_DEPOT = 4;
const int LOCATION_CHARGING = 5;
const int LOCATION_COMMAND = 6;

// Robot capabilities and request categories.
const int CAPABILITY_MEDICAL = 1;
const int CAPABILITY_FIRE = 2;
const int CAPABILITY_SUPPLY = 3;
const int CAPABILITY_SEARCH = 4;
const int CAPABILITY_GENERAL = 5;

const int CATEGORY_MEDICAL = 1;
const int CATEGORY_FIRE = 2;
const int CATEGORY_SUPPLY = 3;
const int CATEGORY_SEARCH = 4;

// Request lifecycle (design report 3.2).
const int REQUEST_UNREPORTED = 1;   // report_time not reached yet
const int REQUEST_PENDING = 2;
const int REQUEST_ASSIGNED = 3;
const int REQUEST_IN_PROGRESS = 4;
const int REQUEST_COMPLETED = 5;
const int REQUEST_FAILED = 6;

// Why a request failed (design report 4.3: only TIME_LIMIT is used by default).
const int FAIL_NONE = 0;
const int FAIL_TIME_LIMIT = 1;

// Robot operational states (design report 3.2).
const int ROBOT_AVAILABLE = 1;
const int ROBOT_TRAVELLING = 2;
const int ROBOT_PERFORMING = 3;
const int ROBOT_RETURNING = 4;     // travelling to a charging station
const int ROBOT_RECHARGING = 5;
const int ROBOT_OFFLINE = 6;
const int ROBOT_UNAVAILABLE = 7;   // online but stuck (blocked road / no usable station / low battery)

// Dynamic events (Appendix A).
const int EVENT_BLOCK = 1;
const int EVENT_UNBLOCK = 2;
const int EVENT_STATION_DOWN = 3;
const int EVENT_STATION_UP = 4;
const int EVENT_SET_URGENCY = 5;
const int EVENT_ROBOT_OFFLINE = 6;
const int EVENT_ROBOT_ONLINE = 7;

// Returned by path searches when a location cannot be reached.
const long long UNREACHABLE_COST = 9000000000000000000LL;

// Plain data records. They hold values only; the logic that uses them lives
// in the modules (environment, parser, ...).
struct Location {
    int id;                    // external ID from the scenario file
    std::string name;
    int type;
    bool station_available;    // meaningful only for LOCATION_CHARGING

    Location() : id(-1), name(""), type(LOCATION_JUNCTION), station_available(false) {}
    Location(int location_id, const std::string& location_name, int location_type)
        : id(location_id), name(location_name), type(location_type),
          station_available(location_type == LOCATION_CHARGING) {}
};

struct Connection {
    int id;                    // external ID from the scenario file
    int endpoint1_id;          // location IDs (undirected road)
    int endpoint2_id;
    int distance;              // positive travel cost
    bool is_blocked;

    Connection() : id(-1), endpoint1_id(-1), endpoint2_id(-1), distance(0), is_blocked(false) {}
    Connection(int connection_id, int first_endpoint, int second_endpoint,
               int connection_distance, bool blocked)
        : id(connection_id), endpoint1_id(first_endpoint), endpoint2_id(second_endpoint),
          distance(connection_distance), is_blocked(blocked) {}
};
