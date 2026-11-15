#pragma once

#include "collections.hpp"
#include "event.hpp"

// =====================================================================
// MEMBER 2 - Min-heap of Events ordered by Event::comes_before.
// Member 3 loads all events from the parser, then pops the ones due at
// each time step (design report section 7: O(log A) per operation).
// =====================================================================
class EventQueue {
private:
    DynamicArray<Event> heap;

    void sift_up(int index);
    void sift_down(int index);

public:
    EventQueue();

    // peek and clear are provided. TODO (Member 2): push, pop, sift_up, sift_down (binary heap in `heap`).
    bool push(const Event& event);
    bool peek(Event& result) const;   // copy of the next event without removing it
    bool pop(Event& result);          // remove and return the next event
    void clear();

    bool empty() const { return heap.empty(); }
    int size() const { return heap.size(); }
};
