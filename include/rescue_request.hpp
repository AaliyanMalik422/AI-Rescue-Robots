#pragma once

#include "types.hpp"

class RescueRequest {
public:
    int id;
    int location_id;
    int category;
    int urgency;
    int report_time;
    int duration_steps;
    int remaining_work;
    int state;
    int assigned_robot_id;

    RescueRequest();
    RescueRequest(int request_id, int location, int request_category,
                  int request_urgency, int time_reported, int work_duration);

    bool assign(int robot_id);
    bool start_work();
    bool perform_work(int time_units);
    bool complete();
    bool fail();
    bool return_to_pending();
};
