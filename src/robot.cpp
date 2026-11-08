#include "../include/robot.hpp"

Robot::Robot() 
    : id(-1), current_location_id(-1), capability(RobotCapability::GENERAL),
      state(RobotState::IDLE), battery_level(100), max_battery(100), assigned_request_id(-1) {}

Robot::Robot(int id_val, int loc_id, RobotCapability cap, int max_batt) 
    : id(id_val), current_location_id(loc_id), capability(cap),
      state(RobotState::IDLE), battery_level(max_batt > 0 ? max_batt : 100), 
      max_battery(max_batt > 0 ? max_batt : 100), assigned_request_id(-1) {}

bool Robot::check_capability(RequestCategory req_cat) const {
    if (capability == RobotCapability::GENERAL) return true;
    if (capability == RobotCapability::MEDICAL && req_cat == RequestCategory::MEDICAL) return true;
    if (capability == RobotCapability::FIRE && req_cat == RequestCategory::FIRE) return true;
    if (capability == RobotCapability::SUPPLY && req_cat == RequestCategory::SUPPLY) return true;
    if (capability == RobotCapability::SEARCH && req_cat == RequestCategory::SEARCH) return true;
    return false;
}

bool Robot::consume_battery(int amount) {
    if (amount <= 0) return false;
    if (battery_level < amount) return false;
    battery_level -= amount;
    return true;
}

bool Robot::charge(int amount) {
    if (amount <= 0 || max_battery <= 0 || battery_level < 0) return false;
    if (battery_level >= max_battery || amount >= max_battery - battery_level) {
        battery_level = max_battery;
    } else {
        battery_level += amount; // Safe because the addition is bounded by max_battery.
    }
    return true;
}

bool Robot::set_state(RobotState new_state) {
    if (new_state == state) return true;

    if (new_state == RobotState::OFFLINE) {
        unassign();
        state = RobotState::OFFLINE;
        return true;
    }

    // An offline robot must explicitly come back online before it can work.
    if (state == RobotState::OFFLINE) {
        if (new_state != RobotState::IDLE) return false;
        state = RobotState::IDLE;
        return true;
    }

    bool allowed = false;
    switch (state) {
        case RobotState::IDLE:
            allowed = (new_state == RobotState::MOVING || new_state == RobotState::CHARGING);
            break;
        case RobotState::MOVING:
            allowed = (new_state == RobotState::RESCUING || new_state == RobotState::CHARGING ||
                       new_state == RobotState::IDLE);
            break;
        case RobotState::RESCUING:
            allowed = (new_state == RobotState::MOVING || new_state == RobotState::CHARGING ||
                       new_state == RobotState::IDLE);
            break;
        case RobotState::CHARGING:
            allowed = (new_state == RobotState::IDLE);
            break;
        case RobotState::OFFLINE:
            allowed = false;
            break;
    }

    if (!allowed) return false;
    state = new_state;
    return true;
}

bool Robot::assign_request(int req_id) {
    if (req_id < 0) return false;
    if (state != RobotState::IDLE) return false;
    if (assigned_request_id != -1) return false;
    if (battery_level <= 0 || max_battery <= 0) return false;

    assigned_request_id = req_id;
    state = RobotState::MOVING;
    return true;
}

void Robot::unassign() {
    assigned_request_id = -1;
    if (state != RobotState::OFFLINE && state != RobotState::CHARGING) {
        state = RobotState::IDLE;
    }
}
