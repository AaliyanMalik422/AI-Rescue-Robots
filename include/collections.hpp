#pragma once

// =====================================================================
// MEMBER 2 - Custom collections (no STL containers).
//
// Interfaces below are the contract used by Member 1 and Member 3.
// Do NOT change a signature without telling them. Bodies marked TODO are
// yours to write. Trivial accessors are already provided.
//
// MERGE THIS FILE FIRST (target: Nov 16). Everyone else builds on it.
// =====================================================================

// ---------------------------------------------------------------------
// DynamicArray<T>: growable array that owns a new[] block.
// Requirements: T has a default constructor and copy assignment.
// Invariants: 0 <= length <= current_capacity; data == 0 iff capacity == 0.
// Expected: push_back amortised O(1), index O(1), remove_at O(n).
// ---------------------------------------------------------------------
template <typename T>
class DynamicArray {
private:
    T* data;
    int length;
    int current_capacity;

    // TODO (Member 2)
    // PSEUDOCODE:
    //   if capacity == 0: new_capacity = 4
    //   else if capacity > 1073741823: return false      (doubling would overflow int)
    //   else new_capacity = capacity * 2
    //   new_block = new T[new_capacity]
    //   for i in 0 .. length-1: new_block[i] = data[i]
    //   delete[] data; data = new_block; capacity = new_capacity
    //   return true
    bool grow() {
        return false;
    }

public:
    DynamicArray() : data(0), length(0), current_capacity(0) {}

    // TODO (Member 2): deep copy (rule of three).
    // PSEUDOCODE: if other.capacity > 0: data = new T[other.capacity];
    //   copy other's elements one by one; length = other.length; capacity = other.capacity
    DynamicArray(const DynamicArray<T>& other)
        : data(0), length(0), current_capacity(0) {
        (void)other;
    }

    // TODO (Member 2): deep copy-assign.
    // PSEUDOCODE: if this == &other: return *this
    //   new_block = (other.capacity > 0) ? new T[other.capacity] : 0
    //   copy other's elements into new_block
    //   delete[] data   (only after the copy succeeded)
    //   data = new_block; length = other.length; capacity = other.capacity
    //   return *this
    DynamicArray<T>& operator=(const DynamicArray<T>& other) {
        (void)other;
        return *this;
    }

    // TODO (Member 2): free the block exactly once.   PSEUDOCODE: delete[] data
    ~DynamicArray() {}

    // TODO (Member 2): append.
    // PSEUDOCODE:
    //   saved = value                       (copy FIRST: value may point into data, and grow() moves data)
    //   if length == capacity and not grow(): return false
    //   data[length] = saved; length = length + 1; return true
    bool push_back(const T& value) {
        (void)value;
        return false;
    }

    // TODO (Member 2): insert before `index` (0..size inclusive).
    // PSEUDOCODE:
    //   if index < 0 or index > length: return false
    //   saved = value
    //   if length == capacity and not grow(): return false
    //   for i from length down to index+1: data[i] = data[i-1]
    //   data[index] = saved; length = length + 1; return true
    bool insert_at(int index, const T& value) {
        (void)index; (void)value;
        return false;
    }

    // DONE (provided).
    bool pop_back() {
        if (length == 0) return false;
        length = length - 1;
        data[length] = T();
        return true;
    }

    // TODO (Member 2): remove element at index.
    // PSEUDOCODE: if index out of 0..length-1: return false
    //   for i from index to length-2: data[i] = data[i+1]
    //   length = length - 1; data[length] = T(); return true
    bool remove_at(int index) {
        (void)index;
        return false;
    }

    // DONE (provided).
    void clear() {
        for (int i = 0; i < length; ++i) data[i] = T();
        length = 0;
    }

    int size() const { return length; }
    int capacity() const { return current_capacity; }
    bool empty() const { return length == 0; }

    // Unchecked on purpose (fast path). Callers check 0 <= index < size().
    T& operator[](int index) { return data[index]; }
    const T& operator[](int index) const { return data[index]; }

    // DONE (provided): checked access, 0 when out of range.
    T* get(int index) {
        if (index < 0 || index >= length) return 0;
        return &data[index];
    }
    const T* get(int index) const {
        if (index < 0 || index >= length) return 0;
        return &data[index];
    }
};

// ---------------------------------------------------------------------
// Singly linked list. The list owns every node it allocates.
// Used for per-location neighbour lists (Member 1 traverses via get_head()).
// Expected: push_front/push_back O(1), pop_back O(n), find O(n).
// ---------------------------------------------------------------------
template <typename T>
struct ListNode {
    T data;
    ListNode<T>* next;

    ListNode(const T& value) : data(value), next(0) {}
};

template <typename T>
class LinkedList {
private:
    ListNode<T>* head;
    ListNode<T>* tail;
    int length;

public:
    LinkedList() : head(0), tail(0), length(0) {}

    // TODO (Member 2): deep copy of every node.
    // PSEUDOCODE: node = other.head; while node != 0: push_back(node->data); node = node->next
    LinkedList(const LinkedList<T>& other) : head(0), tail(0), length(0) {
        (void)other;
    }

    // TODO (Member 2): deep copy-assign; safe for self-assignment, no leaks.
    // PSEUDOCODE: if this == &other: return *this
    //   clear(); node = other.head; while node != 0: push_back(node->data); node = node->next
    LinkedList<T>& operator=(const LinkedList<T>& other) {
        (void)other;
        return *this;
    }

    // TODO (Member 2): delete all nodes.   PSEUDOCODE: clear()
    ~LinkedList() {}

    // TODO (Member 2): each of the following keeps head, tail and length consistent.
    // push_front is DONE (provided) below as a worked example of the node pattern.
    // PSEUDOCODE
    //  push_back : n = new ListNode(value); if tail == 0: head = tail = n else tail->next = n; tail = n; length++
    //  pop_front : if head == 0 return false; old = head; head = head->next; delete old; length--;
    //              if head == 0: tail = 0
    //  pop_back  : if head == 0 return false; if head == tail: delete head; head = tail = 0; length = 0
    //              else walk to the node BEFORE tail; delete tail; tail = that node; tail->next = 0; length--
    //  clear     : while head != 0: pop_front()   (then tail = 0, length = 0)
    bool push_front(const T& value) {
        ListNode<T>* node = new ListNode<T>(value);
        node->next = head;
        head = node;
        if (tail == 0) tail = node;
        length = length + 1;
        return true;
    }
    bool push_back(const T& value) { (void)value; return false; }
    bool pop_front() { return false; }
    bool pop_back() { return false; }
    void clear() {}

    // TODO (Member 2): first node whose data == value, or 0. (T needs operator==.)
    // PSEUDOCODE: node = head; while node != 0: if node->data == value return &node->data; node = node->next; return 0
    T* find(const T& value) { (void)value; return 0; }

    int size() const { return length; }
    bool empty() const { return length == 0; }
    ListNode<T>* get_head() { return head; }
    const ListNode<T>* get_head() const { return head; }
};
