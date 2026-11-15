#include "min_heap.hpp"

// MEMBER 2: indexed binary min-heap. heap[i] has children 2i+1 and 2i+2 and parent (i-1)/2.
// Invariant: position[heap[i].item] == i for every stored entry; position[x] == -1 otherwise.

MinHeap::MinHeap() {}

bool MinHeap::has_higher_priority(const PathNode& first, const PathNode& second) const {
    // DONE (provided)
    if (first.cost != second.cost) return first.cost < second.cost;
    return first.item < second.item;
}

void MinHeap::swap_entries(int first_index, int second_index) {
    // DONE (provided)
    PathNode temporary = heap[first_index];
    heap[first_index] = heap[second_index];
    heap[second_index] = temporary;
    position[heap[first_index].item] = first_index;
    position[heap[second_index].item] = second_index;
}

void MinHeap::heapify_up(int index) {
    // TODO (Member 2)
    // PSEUDOCODE: while index > 0:
    //     parent = (index - 1) / 2
    //     if heap[index] does not beat heap[parent]: stop
    //     swap_entries(index, parent); index = parent
    (void)index;
}

void MinHeap::heapify_down(int index) {
    // TODO (Member 2)
    // PSEUDOCODE: loop:
    //     left = 2*index+1; right = 2*index+2; best = index
    //     if left  < size and heap[left]  beats heap[best]: best = left
    //     if right < size and heap[right] beats heap[best]: best = right
    //     if best == index: stop
    //     swap_entries(index, best); index = best
    (void)index;
}

void MinHeap::reset(int id_capacity) {
    // DONE (provided)
    heap.clear();
    position.clear();
    for (int i = 0; i < id_capacity; ++i) position.push_back(-1);
}

bool MinHeap::insert_or_decrease(int item, long long cost) {
    // TODO (Member 2)
    // PSEUDOCODE:
    //   if item < 0 or item >= position.size(): return false
    //   if position[item] == -1:                      (new item)
    //       push_back PathNode(item, cost) (return false if that fails)
    //       position[item] = heap.size() - 1; heapify_up(heap.size() - 1); return true
    //   else:                                         (already stored)
    //       i = position[item]
    //       if cost < heap[i].cost: heap[i].cost = cost; heapify_up(i)   (smaller cost can only move up)
    //       return true
    (void)item; (void)cost;
    return false;
}

bool MinHeap::extract_min(PathNode& result) {
    // TODO (Member 2)
    // PSEUDOCODE:
    //   if heap is empty: return false
    //   result = heap[0]; position[result.item] = -1
    //   last = heap.size() - 1
    //   if last > 0: heap[0] = heap[last]; position[heap[0].item] = 0
    //   heap.pop_back()
    //   if heap is not empty: heapify_down(0)
    //   return true
    (void)result;
    return false;
}

bool MinHeap::contains(int item) const {
    // DONE (provided)
    return item >= 0 && item < position.size() && position[item] != -1;
}
