#pragma once
#include "collections.hpp"

struct PathNode {
    int location_id;
    long long cost;

    bool operator<(const PathNode& other) const {
        if (cost != other.cost) return cost < other.cost;
        return location_id < other.location_id;
    }
    bool operator>(const PathNode& other) const {
        if (cost != other.cost) return cost > other.cost;
        return location_id > other.location_id;
    }
};

class MinHeap {
private:
    DynamicArray<PathNode> heap;
    void heapify_up(std::size_t index);
    void heapify_down(std::size_t index);

public:
    MinHeap() = default;
    ~MinHeap() = default;

    void insert(const PathNode& node);
    PathNode extract_min();
    bool empty() const;
    std::size_t size() const;
    void clear();
};
