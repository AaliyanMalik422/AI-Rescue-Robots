#include "id_index_map.hpp"

// MEMBER 1: implement insert (find and clear are provided).

bool IdIndexMap::insert(int id, int index) {
    // TODO (Member 1)
    // PSEUDOCODE: if id < 0 or id > MAX_ID: return false
    //   while slots.size() <= id: slots.push_back(-1)       (return false if that fails)
    //   if slots[id] != -1: return false                    (duplicate)
    //   slots[id] = index; count = count + 1; return true
    (void)id; (void)index;
    return false;
}

bool IdIndexMap::find(int id, int& index) const {
    // DONE (provided)
    if (id < 0 || id >= slots.size() || slots[id] == -1) return false;
    index = slots[id];
    return true;
}

void IdIndexMap::clear() {
    // DONE (provided)
    slots.clear();
    count = 0;
}
