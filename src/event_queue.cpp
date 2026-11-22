#include "event_queue.hpp"

EventQueue::EventQueue() {}

void EventQueue::sift_up(int idx) {

    while (idx > 0) {

        int parent = (idx - 1) / 2;
        if (!heap[idx].comes_before(heap[parent])) 
            break;

        Event tmp= heap[idx];
        heap[idx] = heap[parent];
        heap[parent] = tmp;
        idx = parent;
    }
}

void EventQueue::sift_down(int idx) {

    while (true) {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int best = idx;

        if (left < heap.size() && heap[left].comes_before(heap[best]))
            best = left;
        if (right < heap.size() && heap[right].comes_before(heap[best])) 
            best = right;
        if (best == idx)
            break;

        Event tmp = heap[idx];
        heap[idx] = heap[best];
        heap[best] = tmp;
        idx = best;
    }
}

bool EventQueue::push(const Event& event) {

    if (!heap.push_back(event)) 
        return false;
    sift_up(heap.size() - 1);
    return true;
}

bool EventQueue::peek(Event& result) const {

    if (heap.empty()) 
        return false;
    result = heap[0];
    return true;
}

bool EventQueue::pop(Event& result) {

    if (heap.empty()) 
        return false;

    result = heap[0];

    int last = heap.size() - 1;
    heap[0] = heap[last];
    heap.pop_back();

    if (!heap.empty()) 
        sift_down(0);

    return true;
}

void EventQueue::clear() {
    heap.clear();
}