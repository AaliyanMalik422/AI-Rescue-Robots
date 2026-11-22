#include "robot.hpp"


Robot::Robot()
    : id(-1), current_location_id(-1), capability(CAPABILITY_GENERAL), state(ROBOT_AVAILABLE),
    battery_level(0), max_battery(0), assigned_request_id(-1), next_route_index(0),
    current_connection_id(-1), distance_on_current_connection(0), road_target_id(-1),
    mission_target_id(-1), return_station_id(-1), busy_steps(0), online_steps(0), battery_used(0) {
}

Robot::Robot(int robot_id, int start_location, int robot_capability,
    int maximum_battery, int initial_battery)
    : id(robot_id), current_location_id(start_location), capability(robot_capability),
    state(ROBOT_AVAILABLE), battery_level(initial_battery), max_battery(maximum_battery),
    assigned_request_id(-1), next_route_index(0), current_connection_id(-1),
    distance_on_current_connection(0), road_target_id(-1), mission_target_id(-1),
    return_station_id(-1),
    busy_steps(0), online_steps(0), battery_used(0) {
}

bool Robot::check_capability(int request_category) const {
    // DONE (provided)
    if (capability == CAPABILITY_GENERAL) return true;
    if (capability == CAPABILITY_MEDICAL) return request_category == CATEGORY_MEDICAL;
    if (capability == CAPABILITY_FIRE) return request_category == CATEGORY_FIRE;
    if (capability == CAPABILITY_SUPPLY) return request_category == CATEGORY_SUPPLY;
    if (capability == CAPABILITY_SEARCH) return request_category == CATEGORY_SEARCH;
    return false;
}

bool Robot::can_afford(int amount) const {
    // DONE (provided)
    return amount >= 0 && battery_level >= amount;
}

bool Robot::consume_battery(int amount) {
    // DONE (provided)
    if (amount < 0 || battery_level < amount) return false;
    battery_level -= amount;
    battery_used += amount;
    return true;
}

bool Robot::charge(int amount) {
    // DONE (provided)
    if (amount <= 0) return false;
    battery_level += amount;
    if (battery_level > max_battery) battery_level = max_battery;
    return true;
}

bool Robot::set_state(int new_state) {
    if (new_state < ROBOT_AVAILABLE || new_state > ROBOT_UNAVAILABLE) return false;
    if (new_state == state) return true;
    if (new_state == ROBOT_OFFLINE) {
        state = ROBOT_OFFLINE;
        return true;
    }
    if (state == ROBOT_OFFLINE) {
        if (new_state == ROBOT_AVAILABLE || new_state == ROBOT_UNAVAILABLE) {
            state = new_state;
            return true;
        }
        return false;
    }
    bool allowed = false;
    if (state == ROBOT_AVAILABLE) {
        allowed = new_state == ROBOT_TRAVELLING || new_state == ROBOT_RECHARGING ||
            new_state == ROBOT_RETURNING || new_state == ROBOT_UNAVAILABLE;
    }
    else if (state == ROBOT_TRAVELLING) {
        allowed = new_state == ROBOT_PERFORMING || new_state == ROBOT_RETURNING ||
            new_state == ROBOT_AVAILABLE || new_state == ROBOT_UNAVAILABLE;
    }
    else if (state == ROBOT_PERFORMING) {
        allowed = new_state == ROBOT_RETURNING || new_state == ROBOT_AVAILABLE ||
            new_state == ROBOT_UNAVAILABLE;
    }
    else if (state == ROBOT_RETURNING) {
        allowed = new_state == ROBOT_RECHARGING || new_state == ROBOT_AVAILABLE ||
            new_state == ROBOT_UNAVAILABLE;
    }
    else if (state == ROBOT_RECHARGING) {
        allowed = new_state == ROBOT_AVAILABLE || new_state == ROBOT_RETURNING ||
            new_state == ROBOT_UNAVAILABLE;
    }
    else if (state == ROBOT_UNAVAILABLE) {
        allowed = new_state == ROBOT_AVAILABLE || new_state == ROBOT_RETURNING ||
            new_state == ROBOT_RECHARGING;
    }
    if (!allowed) return false;
    state = new_state;
    return true;
}

bool Robot::assign_request(int request_id) {
    if (request_id < 0 || state != ROBOT_AVAILABLE || assigned_request_id != -1) return false;
    assigned_request_id = request_id;
    state = ROBOT_TRAVELLING;
    return true;
}

void Robot::release_request() {
    // DONE (provided)
    assigned_request_id = -1;
    clear_route();
}

bool Robot::set_route(const DynamicArray<int>& route, const DynamicArray<int>& connection_ids) {

    if (route.empty() || connection_ids.size() != route.size() - 1 || is_on_road()) return false;
    planned_route = route;
    planned_connections = connection_ids;
    next_route_index = (route.size() > 1) ? 1 : route.size();
    mission_target_id = route[route.size() - 1];
    return true;
}

bool Robot::is_on_road() const {

    return current_connection_id != -1;
}

void Robot::clear_route() {

    planned_route.clear();
    planned_connections.clear();
    next_route_index = 0;
    mission_target_id = -1;
    return_station_id = -1;
}

int Robot::go_offline() {
    int old = assigned_request_id;
    assigned_request_id = -1;
    clear_route();
    state = ROBOT_OFFLINE;
    return old;
}

bool Robot::go_online() {

    if (state != ROBOT_OFFLINE) 
        return false;
    state = ROBOT_AVAILABLE;
    return true;
}