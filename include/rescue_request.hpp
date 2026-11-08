#pragma once
#include "types.hpp"

class RescueRequest {
public:
    int id;
    int location_id;
    RequestCategory category;
    int urgency;
    int report_time;
    int duration;
    int remaining_work;
    RequestState state;
    int assigned_robot_id;

    RescueRequest();
    RescueRequest(int id_val, int loc_id, RequestCategory cat, int urg, int report_t, int dur);

    bool assign(int robot_id);
    bool start_work();
    bool complete();
    bool fail();

    bool perform_work(int time_units);
};
