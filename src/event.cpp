#include "event.hpp"

bool Event::comes_before(const Event& other) const {
    // DONE (provided): earlier time first, then earlier position in the file.
    if (timestamp != other.timestamp) return timestamp < other.timestamp;
    return original_order < other.original_order;
}
