#pragma once

#include "environment.hpp"

struct PathResult {
    DynamicArray<int> route;
    long long total_cost;
    bool exists;

    PathResult() : total_cost(0), exists(false) {}
};

class Pathfinder {
public:
    // Returns location IDs from start through destination, inclusive.
    // An unreachable or invalid destination produces exists == false.
    static PathResult find_shortest_path(const Environment& environment,
                                         int start_id,
                                         int destination_id);
};
