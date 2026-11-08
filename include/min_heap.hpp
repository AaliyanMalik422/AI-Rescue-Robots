#pragma once

#include "collections.hpp"

struct PathNode {
    int location_id;
    long long cost;

    PathNode() : location_id(-1), cost(0) {}
    PathNode(int id, long long value) : location_id(id), cost(value) {}
};

// Custom binary min-heap. Lower cost has priority; location ID breaks ties.
class MinHeap {
private:
    DynamicArray<PathNode> heap;

    bool has_higher_priority(const PathNode& first, const PathNode& second) const;
    void heapify_up(int index);
    void heapify_down(int index);

public:
    MinHeap();

    bool insert(const PathNode& node);
    bool extract_min(PathNode& result);
    bool empty() const;
    int size() const;
    void clear();
};
