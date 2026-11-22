#pragma once

#include "collections.hpp"



struct PathNode {
    int item;
    long long cost;

    PathNode() : item(-1), cost(0) {}//defualt construtor
    PathNode(int node_item, long long node_cost) : item(node_item), cost(node_cost) {}//parametrized construtor
};

class MinHeap {
private:
    DynamicArray<PathNode> heap;    
    DynamicArray<int> pos;     

    bool has_higher_priority(const PathNode& first, const PathNode& sec) const;
    void heapify_up(int idx);
    void heapify_down(int idx);
    void swap_entries(int first_idx, int sec_idx);

public:
    MinHeap();

    void reset(int id_cap);

    //insert the item, or if it is already present and the new cost is LOWER, decrease its key
    bool insert_or_decrease(int item, long long cost);

    //remove the minimum into `result`
    bool extract_min(PathNode& result);

    //true if item is currently stored.
    bool contains(int item) const;

    bool empty() const { return heap.empty(); }
    int size() const { return heap.size(); }
};
