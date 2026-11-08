#include "min_heap.hpp"

MinHeap::MinHeap() {}

bool MinHeap::has_higher_priority(const PathNode& first, const PathNode& second) const {
    if (first.cost != second.cost) return first.cost < second.cost;
    return first.location_id < second.location_id;
}

void MinHeap::heapify_up(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (!has_higher_priority(heap[index], heap[parent])) break;

        PathNode temporary = heap[index];
        heap[index] = heap[parent];
        heap[parent] = temporary;
        index = parent;
    }
}

void MinHeap::heapify_down(int index) {
    int count = heap.size();

    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int best = index;

        if (left < count && has_higher_priority(heap[left], heap[best])) best = left;
        if (right < count && has_higher_priority(heap[right], heap[best])) best = right;
        if (best == index) break;

        PathNode temporary = heap[index];
        heap[index] = heap[best];
        heap[best] = temporary;
        index = best;
    }
}

bool MinHeap::insert(const PathNode& node) {
    if (!heap.push_back(node)) return false;
    heapify_up(heap.size() - 1);
    return true;
}

bool MinHeap::extract_min(PathNode& result) {
    if (heap.empty()) return false;

    result = heap[0];
    int last_index = heap.size() - 1;
    if (last_index == 0) {
        heap.pop_back();
        return true;
    }

    heap[0] = heap[last_index];
    heap.pop_back();
    heapify_down(0);
    return true;
}

bool MinHeap::empty() const { return heap.empty(); }
int MinHeap::size() const { return heap.size(); }
void MinHeap::clear() { heap.clear(); }
