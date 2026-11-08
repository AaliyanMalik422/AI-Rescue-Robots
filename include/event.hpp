#pragma once

#include "types.hpp"

class Event {
public:
    int type;
    int timestamp;
    int target_id;
    int value;
    int original_order;

    Event();
    Event(int event_type, int time, int target, int event_value, int order);
};
