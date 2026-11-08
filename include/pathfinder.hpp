#pragma once
#include "environment.hpp"
#include "collections.hpp"

struct PathResult {
    DynamicArray<int> route;
    long long total_cost;
    bool exists;
};

class Pathfinder {
public:
    static PathResult find_shortest_path(const Environment& env, int start_id, int end_id);
};
