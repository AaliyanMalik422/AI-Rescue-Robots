#include "../include/min_heap.hpp"
#include <stdexcept>

void MinHeap::heapify_up(size_t index) {
    while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (heap[index] < heap[parent]) {
            PathNode temp = heap[index];
            heap[index] = heap[parent];
            heap[parent] = temp;
            index = parent;
        } else { break; }
    }
}

void MinHeap::heapify_down(size_t index) {
    size_t size = heap.size();
    while (true) {
        size_t left = 2 * index + 1;
        size_t right = 2 * index + 2;
        size_t smallest = index;
        
        if (left < size && heap[left] < heap[smallest]) smallest = left;
        if (right < size && heap[right] < heap[smallest]) smallest = right;
        
        if (smallest != index) {
            PathNode temp = heap[index];
            heap[index] = heap[smallest];
            heap[smallest] = temp;
            index = smallest;
        } else { break; }
    }
}

void MinHeap::insert(const PathNode& node) {
    heap.push_back(node);
    heapify_up(heap.size() - 1);
}

PathNode MinHeap::extract_min() {
    if (empty()) throw std::out_of_range("Heap is empty");
    PathNode min_val = heap[0];
    heap[0] = heap[heap.size() - 1];
    heap.pop_back();
    if (!heap.empty()) heapify_down(0);
    return min_val;
}

bool MinHeap::empty() const { return heap.empty(); }
size_t MinHeap::size() const { return heap.size(); }
void MinHeap::clear() { heap.clear(); }
