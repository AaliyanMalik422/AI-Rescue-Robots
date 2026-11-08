#pragma once
#include "types.hpp"

class Event {
public:
    EventType type;
    int timestamp;
    int target_id;
    int value;
    int original_order;
    
    Event();
    Event(EventType t, int time, int target, int val, int order);
};
