#pragma once
#include "types.hpp"

class Robot {
public:
    int id;
    int current_location_id;
    RobotCapability capability;
    RobotState state;
    int battery_level;
    int max_battery;
    int assigned_request_id;

    Robot();
    Robot(int id_val, int loc_id, RobotCapability cap, int max_batt);

    bool check_capability(RequestCategory req_cat) const;
    bool consume_battery(int amount);
    bool charge(int amount);

    bool set_state(RobotState new_state);
    bool assign_request(int req_id);
    void unassign();
};
