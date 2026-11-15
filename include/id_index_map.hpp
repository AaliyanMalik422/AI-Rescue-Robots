#pragma once

#include "collections.hpp"

// =====================================================================
// MEMBER 1 - Direct-address map from an external ID to an array index.
//
// Replaces the hash table of the Phase 1 report (see docs/design_blueprint.md,
// "Deviations"). It is just an array: slots[id] holds the index, or -1.
// Lookup and insert are O(1) with no hashing or collisions.
// Space is O(max ID); the parser must reject IDs above MAX_ID so a
// malformed file cannot request a huge array.
// =====================================================================
const int MAX_ID = 1000000;

class IdIndexMap {
private:
    DynamicArray<int> slots;   // slots[id] = index, or -1 when the ID is unused
    int count;

public:
    IdIndexMap() : count(0) {}

    // TODO (Member 1): reject id < 0, id > MAX_ID, or an ID already present.
    // Grow `slots` so slots[id] exists (fill new entries with -1), then store index.
    bool insert(int id, int index);

    // DONE (provided): true and sets `index` if the ID is present.
    bool find(int id, int& index) const;

    // DONE (provided): forget every ID.
    void clear();

    int size() const { return count; }
};
