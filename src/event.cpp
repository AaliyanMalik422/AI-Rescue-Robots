#include "event.hpp"

Event::Event()
    : type(EVENT_BLOCK), timestamp(0), target_id(-1), value(0), original_order(0) {}

Event::Event(int event_type, int time, int target, int event_value, int order)
    : type(event_type), timestamp(time), target_id(target), value(event_value), original_order(order) {}
