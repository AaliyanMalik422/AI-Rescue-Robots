#pragma once

#include "types.hpp"

// =====================================================================
// MEMBER 2 - Event record (data only; value is the new urgency for
// SET_URGENCY, unused otherwise).
// =====================================================================
class Event {
public:
    int type;
    int timestamp;
    int target_id;        // connection / location / request / robot ID
    int value;
    int original_order;   // position in the file; breaks timestamp ties

    Event() : type(EVENT_BLOCK), timestamp(0), target_id(-1), value(0), original_order(0) {}
    Event(int event_type, int time, int target, int event_value, int order)
        : type(event_type), timestamp(time), target_id(target),
          value(event_value), original_order(order) {}

    // DONE (provided): true if this event must be processed before `other`:
    // earlier timestamp first, then lower original_order.
    bool comes_before(const Event& other) const;
};
