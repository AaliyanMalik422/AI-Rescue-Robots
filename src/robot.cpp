#include "robot.hpp"

namespace {
bool valid_capability(int capability) {
    return capability == CAPABILITY_MEDICAL || capability == CAPABILITY_FIRE ||
           capability == CAPABILITY_SUPPLY || capability == CAPABILITY_SEARCH ||
           capability == CAPABILITY_GENERAL;
}

bool valid_robot_state(int state) {
    return state >= ROBOT_IDLE && state <= ROBOT_OFFLINE;
}
}

Robot::Robot()
    : id(-1), current_location_id(-1), capability(CAPABILITY_GENERAL), state(ROBOT_IDLE),
      battery_level(100), max_battery(100), assigned_request_id(-1), next_route_index(0),
      current_connection_id(-1), distance_on_current_connection(0), mission_target_id(-1),
      return_station_id(-1) {}

Robot::Robot(int robot_id, int start_location, int robot_capability,
             int maximum_battery, int initial_battery)
    : id(robot_id), current_location_id(start_location), capability(robot_capability),
      state(ROBOT_IDLE), battery_level(initial_battery), max_battery(maximum_battery),
      assigned_request_id(-1), next_route_index(0), current_connection_id(-1),
      distance_on_current_connection(0), mission_target_id(-1), return_station_id(-1) {
    if (id < 0 || current_location_id < 0 || !valid_capability(capability) ||
        max_battery <= 0 || battery_level < 0 || battery_level > max_battery) {
        id = -1;
        current_location_id = -1;
        capability = CAPABILITY_GENERAL;
        max_battery = 100;
        battery_level = 0;
        state = ROBOT_OFFLINE;
    }
}

bool Robot::check_capability(int request_category) const {
    if (capability == CAPABILITY_GENERAL) return true;
    if (capability == CAPABILITY_MEDICAL && request_category == CATEGORY_MEDICAL) return true;
    if (capability == CAPABILITY_FIRE && request_category == CATEGORY_FIRE) return true;
    if (capability == CAPABILITY_SUPPLY && request_category == CATEGORY_SUPPLY) return true;
    if (capability == CAPABILITY_SEARCH && request_category == CATEGORY_SEARCH) return true;
    return false;
}

bool Robot::consume_battery(int amount) {
    if (amount < 0 || battery_level < amount) return false;
    battery_level -= amount;
    return true;
}

bool Robot::charge(int amount) {
    if (amount <= 0 || max_battery <= 0 || battery_level < 0) return false;
    if (battery_level >= max_battery) return true;

    int remaining_capacity = max_battery - battery_level;
    if (amount >= remaining_capacity) battery_level = max_battery;
    else battery_level += amount;
    return true;
}

bool Robot::set_state(int new_state) {
    if (!valid_robot_state(new_state)) return false;
    if (new_state == state) return true;

    if (new_state == ROBOT_OFFLINE) {
        assigned_request_id = -1;
        clear_route();
        state = ROBOT_OFFLINE;
        return true;
    }

    if (state == ROBOT_OFFLINE) {
        if (new_state != ROBOT_IDLE) return false;
        state = ROBOT_IDLE;
        return true;
    }

    bool allowed = false;
    if (state == ROBOT_IDLE) {
        allowed = new_state == ROBOT_MOVING || new_state == ROBOT_CHARGING;
    } else if (state == ROBOT_MOVING) {
        allowed = new_state == ROBOT_RESCUING || new_state == ROBOT_RETURNING ||
                  new_state == ROBOT_IDLE;
    } else if (state == ROBOT_RESCUING) {
        allowed = new_state == ROBOT_RETURNING || new_state == ROBOT_IDLE;
    } else if (state == ROBOT_RETURNING) {
        allowed = new_state == ROBOT_CHARGING || new_state == ROBOT_IDLE;
    } else if (state == ROBOT_CHARGING) {
        allowed = new_state == ROBOT_IDLE;
    }

    if (!allowed) return false;
    state = new_state;
    return true;
}

bool Robot::assign_request(int request_id) {
    if (request_id < 0 || state != ROBOT_IDLE || assigned_request_id != -1) return false;
    if (battery_level <= 0 || max_battery <= 0) return false;

    assigned_request_id = request_id;
    state = ROBOT_MOVING;
    return true;
}

void Robot::release_request() {
    assigned_request_id = -1;
    if (state == ROBOT_MOVING || state == ROBOT_RESCUING) state = ROBOT_IDLE;
}

bool Robot::set_route(const DynamicArray<int>& route) {
    if (route.empty()) return false;

    planned_route = route;
    next_route_index = route.size() > 1 ? 1 : route.size();
    current_connection_id = -1;
    distance_on_current_connection = 0;
    mission_target_id = route[route.size() - 1];
    return true;
}

void Robot::clear_route() {
    planned_route.clear();
    next_route_index = 0;
    current_connection_id = -1;
    distance_on_current_connection = 0;
    mission_target_id = -1;
    return_station_id = -1;
}
