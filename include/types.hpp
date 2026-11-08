#pragma once
#include <string>

enum class RobotCapability { MEDICAL, FIRE, SUPPLY, SEARCH, GENERAL };
enum class RequestCategory { MEDICAL, FIRE, SUPPLY, SEARCH };
enum class RequestState { PENDING, ASSIGNED, IN_PROGRESS, COMPLETED, FAILED };
enum class RobotState { IDLE, MOVING, RESCUING, CHARGING, OFFLINE };
enum class EventType { BLOCK, UNBLOCK, STATION_DOWN, STATION_UP, SET_URGENCY, ROBOT_OFFLINE, ROBOT_ONLINE };

struct Location {
    int id;
    std::string name;
    bool has_charging_station;
    bool is_station_available;
};

struct Connection {
    int id;
    int endpoint1_id;
    int endpoint2_id;
    int distance;
    bool is_blocked;
};
