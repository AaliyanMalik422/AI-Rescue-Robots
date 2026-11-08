#pragma once

#include "types.hpp"
#include "collections.hpp"

class Robot {
public:
    int id;
    int current_location_id;
    int capability;
    int state;
    int battery_level;
    int max_battery;
    int assigned_request_id;

    // Route progress is stored so Member 3 can implement multi-step movement.
    // next_route_index is the index of the next location to travel to.
    DynamicArray<int> planned_route;
    int next_route_index;
    int current_connection_id;
    int distance_on_current_connection;
    int mission_target_id;
    int return_station_id;

    Robot();
    Robot(int robot_id, int start_location, int robot_capability,
          int maximum_battery, int initial_battery);

    bool check_capability(int request_category) const;
    bool consume_battery(int amount);
    bool charge(int amount);
    bool set_state(int new_state);
    bool assign_request(int request_id);
    void release_request();
    bool set_route(const DynamicArray<int>& route);
    void clear_route();
};
