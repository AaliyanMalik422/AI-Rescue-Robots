#pragma once

#include "collections.hpp"
#include "event.hpp"

class EventQueue {
private:
    DynamicArray<Event> heap;

    void sift_up(int idx);
    void sift_down(int idx);

public:
    EventQueue();

    bool push(const Event& event);
    bool peek(Event& result) const;   
    bool pop(Event& result);          
    void clear();

    bool empty() const { return heap.empty(); }
    int size() const { return heap.size(); }
};
