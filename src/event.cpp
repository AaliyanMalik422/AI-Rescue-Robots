#include "../include/event.hpp"

Event::Event() : type(EventType::BLOCK), timestamp(0), target_id(-1), value(0), original_order(0) {}

Event::Event(EventType t, int time, int target, int val, int order)
    : type(t), timestamp(time), target_id(target), value(val), original_order(order) {}
