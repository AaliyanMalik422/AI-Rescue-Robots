#include "event.hpp"

bool Event::comes_before(const Event& other) const {

    if (timestamp != other.timestamp) 
        return timestamp < other.timestamp;

    return original_order < other.original_order;
}
