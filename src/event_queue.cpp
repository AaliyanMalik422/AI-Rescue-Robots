#include "event_queue.hpp"

// MEMBER 2: binary min-heap of events; "smaller" means Event::comes_before.
// Same index arithmetic as MinHeap, but no position array is needed.

EventQueue::EventQueue() {}

void EventQueue::sift_up(int index) {
    // TODO (Member 2)
    // PSEUDOCODE: while index > 0: parent = (index-1)/2;
    //   if heap[index].comes_before(heap[parent]): swap them; index = parent  else stop
    (void)index;
}

void EventQueue::sift_down(int index) {
    // TODO (Member 2)
    // PSEUDOCODE: loop: best = index; check left (2i+1) and right (2i+2) with comes_before;
    //   if best == index stop; swap(index, best); index = best
    (void)index;
}

bool EventQueue::push(const Event& event) {
    // TODO (Member 2)
    // PSEUDOCODE: if not heap.push_back(event): return false; sift_up(heap.size() - 1); return true
    (void)event;
    return false;
}

bool EventQueue::peek(Event& result) const {
    // DONE (provided)
    if (heap.empty()) return false;
    result = heap[0];
    return true;
}

bool EventQueue::pop(Event& result) {
    // TODO (Member 2)
    // PSEUDOCODE: if empty: return false; result = heap[0];
    //   heap[0] = heap[last]; heap.pop_back(); if not empty: sift_down(0); return true
    (void)result;
    return false;
}

void EventQueue::clear() {
    // DONE (provided)
    heap.clear();
}
