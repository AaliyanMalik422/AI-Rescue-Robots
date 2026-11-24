#include "id_index_map.hpp"

// MEMBER 1: direct-address ID to array-index map.

bool IdIndexMap::insert(int id, int index) {
    if (id < 0 || id > MAX_ID || index < 0) return false;

    // Grow the direct-address array and mark every new slot as unused.
    while (slots.size() <= id) {
        if (!slots.push_back(-1)) return false;
    }

    if (slots[id] != -1) return false;

    slots[id] = index;
    ++count;
    return true;
}

bool IdIndexMap::find(int id, int& index) const {
    if (id < 0 || id >= slots.size() || slots[id] == -1) return false;
    index = slots[id];
    return true;
}

void IdIndexMap::clear() {
    slots.clear();
    count = 0;
}
