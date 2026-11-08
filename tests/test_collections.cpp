#include <iostream>
#include <utility>
#include "../include/collections.hpp"
#include "../include/min_heap.hpp"

namespace {
struct LifetimeProbe {
    static int alive;
    int value;
    explicit LifetimeProbe(int v = 0) : value(v) { ++alive; }
    LifetimeProbe(const LifetimeProbe& other) : value(other.value) { ++alive; }
    LifetimeProbe(LifetimeProbe&& other) noexcept : value(other.value) { other.value = -1; ++alive; }
    LifetimeProbe& operator=(const LifetimeProbe& other) { value = other.value; return *this; }
    LifetimeProbe& operator=(LifetimeProbe&& other) noexcept { value = other.value; other.value = -1; return *this; }
    ~LifetimeProbe() { --alive; }
};
int LifetimeProbe::alive = 0;

struct ThrowingCopy {
    static int alive;
    static int copies_before_throw;
    int value;
    explicit ThrowingCopy(int v = 0) : value(v) { ++alive; }
    ThrowingCopy(const ThrowingCopy& other) : value(other.value) {
        if (copies_before_throw == 0) throw 42;
        if (copies_before_throw > 0) --copies_before_throw;
        ++alive;
    }
    ThrowingCopy(ThrowingCopy&& other) noexcept : value(other.value) { other.value = -1; ++alive; }
    ThrowingCopy& operator=(const ThrowingCopy& other) { value = other.value; return *this; }
    ThrowingCopy& operator=(ThrowingCopy&& other) noexcept { value = other.value; other.value = -1; return *this; }
    ~ThrowingCopy() { --alive; }
};
int ThrowingCopy::alive = 0;
int ThrowingCopy::copies_before_throw = -1;
}

int run_collections_tests() {
    int fails = 0;
    try {
        DynamicArray<int> arr;
        arr.push_back(10);
        arr.push_back(20);
        DynamicArray<int> arr2 = arr;
        arr2.push_back(30);
        if (arr.size() != 2 || arr2.size() != 3 || arr[0] != 10 || arr2[2] != 30) ++fails;

        DynamicArray<int> arr3;
        arr3 = arr2;
        arr3 = arr3;
        if (arr3.size() != 3 || arr3[0] != 10) ++fails;

        DynamicArray<int> arr4 = std::move(arr3);
        if (arr4.size() != 3 || !arr3.empty()) ++fails;
        DynamicArray<int> arr5;
        arr5 = std::move(arr4);
        if (arr5.size() != 3 || !arr4.empty()) ++fails;

        // A const reference to an existing element must survive a growth reallocation.
        DynamicArray<int> alias;
        alias.push_back(41);
        alias.push_back(42);
        alias.push_back(alias[0]);
        if (alias.size() != 3 || alias[0] != 41 || alias[2] != 41) ++fails;

        arr5.pop_back();
        if (arr5.size() != 2) ++fails;
        arr5.clear();
        if (!arr5.empty()) ++fails;
        bool bounds_threw = false;
        try { (void)arr5[0]; } catch (const std::out_of_range&) { bounds_threw = true; }
        if (!bounds_threw) ++fails;

        // Verify that pop_back/clear destroy elements immediately.
        if (LifetimeProbe::alive != 0) ++fails;
        {
            DynamicArray<LifetimeProbe> probes;
            probes.push_back(LifetimeProbe(1));
            probes.push_back(LifetimeProbe(2));
            if (LifetimeProbe::alive != 2) ++fails;
            probes.pop_back();
            if (LifetimeProbe::alive != 1) ++fails;
            probes.clear();
            if (LifetimeProbe::alive != 0) ++fails;
        }
        if (LifetimeProbe::alive != 0) ++fails;

        // Failed copies must clean up partially constructed storage.
        {
            ThrowingCopy first(1);
            ThrowingCopy second(2);
            DynamicArray<ThrowingCopy> source;
            source.push_back(first);
            source.push_back(second);
            const int before = ThrowingCopy::alive;
            ThrowingCopy::copies_before_throw = 1;
            bool threw = false;
            try { DynamicArray<ThrowingCopy> copy(source); }
            catch (int) { threw = true; }
            ThrowingCopy::copies_before_throw = -1;
            if (!threw || ThrowingCopy::alive != before) ++fails;
        }
        if (ThrowingCopy::alive != 0) ++fails;

        // Copy assignment keeps its original value when construction of the
        // temporary copy fails part-way through.
        {
            ThrowingCopy source_one(11), source_two(22), destination_value(77);
            DynamicArray<ThrowingCopy> source;
            source.push_back(source_one);
            source.push_back(source_two);
            DynamicArray<ThrowingCopy> destination;
            destination.push_back(destination_value);
            const int before = ThrowingCopy::alive;
            ThrowingCopy::copies_before_throw = 1;
            bool threw = false;
            try { destination = source; }
            catch (int) { threw = true; }
            ThrowingCopy::copies_before_throw = -1;
            if (!threw || destination.size() != 1 || destination[0].value != 77 ||
                ThrowingCopy::alive != before) ++fails;
        }
        if (ThrowingCopy::alive != 0) ++fails;

        LinkedList<int> list;
        list.pop_front();
        list.pop_back();
        list.push_back(1);
        list.push_front(2);
        if (list.size() != 2) ++fails;

        LinkedList<int> list_copy = list;
        if (list_copy.size() != 2 || !list_copy.find(1) || *list_copy.find(1) != 1) ++fails;
        LinkedList<int> assigned;
        assigned.push_back(99);
        assigned = list_copy;
        assigned = assigned;
        if (assigned.size() != 2 || !assigned.find(2)) ++fails;

        LinkedList<int> moved(std::move(assigned));
        if (!assigned.empty() || moved.size() != 2) ++fails;
        LinkedList<int> move_assigned;
        move_assigned = std::move(moved);
        if (!moved.empty() || move_assigned.size() != 2) ++fails;

        list.pop_back();
        list.pop_front();
        if (!list.empty()) ++fails;
        move_assigned.clear();
        if (!move_assigned.empty()) ++fails;

        // Copy failure must leave the original destination intact and leak no nodes.
        {
            ThrowingCopy a(10), b(20);
            LinkedList<ThrowingCopy> original;
            original.push_back(a);
            original.push_back(b);
            LinkedList<ThrowingCopy> destination;
            destination.push_back(a);
            const int before = ThrowingCopy::alive;
            ThrowingCopy::copies_before_throw = 1;
            bool threw = false;
            try { destination = original; }
            catch (int) { threw = true; }
            ThrowingCopy::copies_before_throw = -1;
            if (!threw || ThrowingCopy::alive != before || destination.size() != 1) ++fails;
        }
        if (ThrowingCopy::alive != 0) ++fails;

        // A throwing LinkedList copy constructor must release every node it
        // managed to create before propagating the exception.
        {
            ThrowingCopy a(31), b(32);
            LinkedList<ThrowingCopy> original;
            original.push_back(a);
            original.push_back(b);
            const int before = ThrowingCopy::alive;
            ThrowingCopy::copies_before_throw = 1;
            bool threw = false;
            try { LinkedList<ThrowingCopy> copy(original); }
            catch (int) { threw = true; }
            ThrowingCopy::copies_before_throw = -1;
            if (!threw || ThrowingCopy::alive != before) ++fails;
        }
        if (ThrowingCopy::alive != 0) ++fails;

        MinHeap heap;
        heap.insert({1, 100});
        heap.insert({2, 50});
        heap.insert({3, 50});
        if (heap.empty() || heap.size() != 3) ++fails;
        const PathNode m1 = heap.extract_min();
        const PathNode m2 = heap.extract_min();
        const PathNode m3 = heap.extract_min();
        if (m1.cost != 50 || m2.cost != 50 || m3.cost != 100) ++fails;
        if (m1.location_id != 2 || m2.location_id != 3) ++fails;
        bool empty_heap_threw = false;
        try { (void)heap.extract_min(); } catch (const std::out_of_range&) { empty_heap_threw = true; }
        if (!empty_heap_threw) ++fails;
    } catch (...) {
        ++fails;
    }

    if (fails) std::cerr << "[FAIL] Collections tests (" << fails << " failures).\n";
    else std::cout << "[PASS] Collections tests.\n";
    return fails > 0 ? 1 : 0;
}
