#pragma once

#include "collections.hpp"

// =====================================================================
// MEMBER 2 - Indexed binary min-heap used by Dijkstra (Member 1).
//
// Design report 4.1: the heap holds at most ONE entry per item, and a
// position array makes decrease-key O(log n).
//
// "item" is a small integer in [0, id_capacity). Member 1 will use
// location INDICES (0..V-1), not external location IDs.
// Priority: lower cost first; on equal cost, lower item first.
// =====================================================================

struct PathNode {
    int item;
    long long cost;

    PathNode() : item(-1), cost(0) {}
    PathNode(int node_item, long long node_cost) : item(node_item), cost(node_cost) {}
};

class MinHeap {
private:
    DynamicArray<PathNode> heap;     // heap-ordered entries
    DynamicArray<int> position;      // position[item] = index in heap, or -1

    // DONE (provided): comparison with the tie-break above.
    bool has_higher_priority(const PathNode& first, const PathNode& second) const;
    // swap_entries is provided (it keeps `position` in sync). TODO (Member 2): heapify_up / heapify_down.
    void heapify_up(int index);
    void heapify_down(int index);
    void swap_entries(int first_index, int second_index);

public:
    MinHeap();

    // DONE (provided): empty the heap and make `position` hold id_capacity
    // entries, all -1. Call before every Dijkstra run.
    void reset(int id_capacity);

    // TODO (Member 2): insert the item, or if it is already present and the new
    // cost is LOWER, decrease its key. Return false for an item out of range
    // or when memory fails. A higher cost for a present item is ignored (true).
    bool insert_or_decrease(int item, long long cost);

    // TODO (Member 2): remove the minimum into `result`. False if empty.
    bool extract_min(PathNode& result);

    // DONE (provided): true if item is currently stored.
    bool contains(int item) const;

    bool empty() const { return heap.empty(); }
    int size() const { return heap.size(); }
};
