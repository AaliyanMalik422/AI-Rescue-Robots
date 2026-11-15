#include "robot.hpp"

// MEMBER 2: robot rules. Constructors only store values; validating scenario
// data is the parser's job (Member 1).

Robot::Robot()
    : id(-1), current_location_id(-1), capability(CAPABILITY_GENERAL), state(ROBOT_AVAILABLE),
      battery_level(0), max_battery(0), assigned_request_id(-1), next_route_index(0),
      current_connection_id(-1), distance_on_current_connection(0), road_target_id(-1),
      mission_target_id(-1), return_station_id(-1), busy_steps(0), online_steps(0), battery_used(0) {}

Robot::Robot(int robot_id, int start_location, int robot_capability,
             int maximum_battery, int initial_battery)
    : id(robot_id), current_location_id(start_location), capability(robot_capability),
      state(ROBOT_AVAILABLE), battery_level(initial_battery), max_battery(maximum_battery),
      assigned_request_id(-1), next_route_index(0), current_connection_id(-1),
      distance_on_current_connection(0), road_target_id(-1), mission_target_id(-1),
      return_station_id(-1),
      busy_steps(0), online_steps(0), battery_used(0) {}

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
    // TODO (Member 2): transition table and pseudocode are in robot.hpp.
    (void)new_state;
    return false;
}

bool Robot::assign_request(int request_id) {
    // TODO (Member 2): pseudocode is in robot.hpp.
    (void)request_id;
    return false;
}

void Robot::release_request() {
    // DONE (provided)
    assigned_request_id = -1;
    clear_route();
}

bool Robot::set_route(const DynamicArray<int>& route, const DynamicArray<int>& connection_ids) {
    // TODO (Member 2): see robot.hpp for the pseudocode.
    (void)route; (void)connection_ids;
    return false;
}

bool Robot::is_on_road() const {
    // DONE (provided)
    return current_connection_id != -1;
}

void Robot::clear_route() {
    // DONE (provided): forgets the route but keeps any mid-road progress.
    planned_route.clear();
    planned_connections.clear();
    next_route_index = 0;
    mission_target_id = -1;
    return_station_id = -1;
}

int Robot::go_offline() {
    // TODO (Member 2): pseudocode is in robot.hpp.
    return -1;
}

bool Robot::go_online() {
    // DONE (provided): never resumes an old task.
    if (state != ROBOT_OFFLINE) return false;
    state = ROBOT_AVAILABLE;
    return true;
}
