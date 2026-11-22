#include "min_heap.hpp"

MinHeap::MinHeap() {}

bool MinHeap::has_higher_priority(const PathNode& first, const PathNode& sec) const {

    if (first.cost != sec.cost) 
        return first.cost < sec.cost;
    return first.item < sec.item;
}

void MinHeap::swap_entries(int first_idx, int sec_idx) {

    PathNode tmp = heap[first_idx];
    heap[first_idx] = heap[sec_idx];
    heap[sec_idx] = tmp;
    pos[heap[first_idx].item] = first_idx;
    pos[heap[sec_idx].item] = sec_idx;
}

void MinHeap::heapify_up(int idx) {

    while (idx > 0) {

        int parent = (idx - 1) / 2;

        if ( !has_higher_priority(heap[idx], heap[parent]) ) 
            break;

        swap_entries(idx, parent);
        idx = parent;
    }
}

void MinHeap::heapify_down(int idx) {

    while (true) {

        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int best = idx;

        if (left < heap.size() && has_higher_priority(heap[left], heap[best])) 
            best = left;
        if (right < heap.size() && has_higher_priority(heap[right], heap[best])) 
            best = right;
        if (best == idx) 
            break;

        swap_entries(idx, best);
        idx = best;
    }
}

void MinHeap::reset(int id_cap) {

    heap.clear();
    pos.clear();
    for (int i = 0; i < id_cap; ++i) pos.push_back(-1);
}

bool MinHeap::insert_or_decrease(int item, long long cost) {
   
    if (item < 0 || item >= pos.size())//invalid item
        return false;
    if (pos[item] == -1) {//empty

        if (!heap.push_back(PathNode(item, cost))) //insert
            return false;

        pos[item] = heap.size() - 1;
        heapify_up(heap.size() - 1);
        return true;
    }
    int i = pos[item];
    if (cost < heap[i].cost) {//decrease
        heap[i].cost = cost;
        heapify_up(i);
    }
    return true;
}

bool MinHeap::extract_min(PathNode& result) {
    
    if (heap.empty()) 
        return false;

    result = heap[0];
    pos[result.item] = -1;
    int last = heap.size() - 1;

    if (last > 0) {
        heap[0] = heap[last];
        pos[heap[0].item] = 0;
    }
    heap.pop_back();
    if (!heap.empty()) 
        heapify_down(0);
    return true;
}

bool MinHeap::contains(int item) const {
    return item >= 0 && item < pos.size() && pos[item] != -1;
}