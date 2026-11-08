#pragma once

// Student-built data structures.
//
// DynamicArray uses new[]/delete[] and therefore requires T to have a default
// constructor and copy-assignment operator. This is suitable for the project
// types (Location, Connection, Robot, RescueRequest, Event, and simple values).

template <typename T>
class DynamicArray {
private:
    T* data;
    int length;
    int current_capacity;

    bool grow() {
        int new_capacity;

        if (current_capacity == 0) {
            new_capacity = 4;
        } else {
            // Prevent signed integer overflow when doubling the capacity.
            if (current_capacity > 1073741823) return false;
            new_capacity = current_capacity * 2;
        }

        T* new_data = new T[new_capacity];
        for (int i = 0; i < length; ++i) {
            new_data[i] = data[i];
        }

        delete[] data;
        data = new_data;
        current_capacity = new_capacity;
        return true;
    }

public:
    DynamicArray() : data(0), length(0), current_capacity(0) {}

    DynamicArray(const DynamicArray<T>& other)
        : data(0), length(0), current_capacity(0) {
        if (other.current_capacity > 0) {
            data = new T[other.current_capacity];
            current_capacity = other.current_capacity;
            for (int i = 0; i < other.length; ++i) {
                data[i] = other.data[i];
            }
            length = other.length;
        }
    }

    DynamicArray<T>& operator=(const DynamicArray<T>& other) {
        if (this != &other) {
            T* new_data = 0;
            if (other.current_capacity > 0) {
                new_data = new T[other.current_capacity];
                for (int i = 0; i < other.length; ++i) {
                    new_data[i] = other.data[i];
                }
            }

            delete[] data;
            data = new_data;
            length = other.length;
            current_capacity = other.current_capacity;
        }
        return *this;
    }

    ~DynamicArray() {
        delete[] data;
    }

    bool push_back(const T& value) {
        // Copy before growing. This makes push_back(array[i]) safe even when
        // the array is full and grow() replaces the backing storage.
        T saved_value = value;

        if (length == current_capacity && !grow()) {
            return false;
        }

        data[length] = saved_value;
        ++length;
        return true;
    }

    bool pop_back() {
        if (length == 0) return false;
        --length;
        data[length] = T();
        return true;
    }

    bool remove_at(int index) {
        if (index < 0 || index >= length) return false;

        for (int i = index; i < length - 1; ++i) {
            data[i] = data[i + 1];
        }

        --length;
        data[length] = T();
        return true;
    }

    void clear() {
        for (int i = 0; i < length; ++i) {
            data[i] = T();
        }
        length = 0;
    }

    int size() const { return length; }
    int capacity() const { return current_capacity; }
    bool empty() const { return length == 0; }

    // operator[] is intentionally unchecked. Check 0 <= index < size() first.
    T& operator[](int index) { return data[index]; }
    const T& operator[](int index) const { return data[index]; }

    T* get(int index) {
        if (index < 0 || index >= length) return 0;
        return &data[index];
    }

    const T* get(int index) const {
        if (index < 0 || index >= length) return 0;
        return &data[index];
    }
};

// Singly linked list node. The list owns and deletes every node it creates.
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

    LinkedList(const LinkedList<T>& other) : head(0), tail(0), length(0) {
        const ListNode<T>* current = other.head;
        while (current != 0) {
            push_back(current->data);
            current = current->next;
        }
    }

    LinkedList<T>& operator=(const LinkedList<T>& other) {
        if (this != &other) {
            LinkedList<T> copy(other);
            clear();
            head = copy.head;
            tail = copy.tail;
            length = copy.length;

            // Transfer node ownership from the temporary copy.
            copy.head = 0;
            copy.tail = 0;
            copy.length = 0;
        }
        return *this;
    }

    ~LinkedList() { clear(); }

    bool push_front(const T& value) {
        ListNode<T>* node = new ListNode<T>(value);
        node->next = head;
        head = node;
        if (tail == 0) tail = node;
        ++length;
        return true;
    }

    bool push_back(const T& value) {
        ListNode<T>* node = new ListNode<T>(value);
        if (tail == 0) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        ++length;
        return true;
    }

    bool pop_front() {
        if (head == 0) return false;

        ListNode<T>* removed = head;
        head = head->next;
        delete removed;
        --length;

        if (head == 0) tail = 0;
        return true;
    }

    bool pop_back() {
        if (head == 0) return false;

        if (head == tail) {
            delete head;
            head = 0;
            tail = 0;
            length = 0;
            return true;
        }

        ListNode<T>* previous = head;
        while (previous->next != tail) {
            previous = previous->next;
        }

        delete tail;
        tail = previous;
        tail->next = 0;
        --length;
        return true;
    }

    void clear() {
        while (head != 0) {
            ListNode<T>* removed = head;
            head = head->next;
            delete removed;
        }
        tail = 0;
        length = 0;
    }

    int size() const { return length; }
    bool empty() const { return length == 0; }

    ListNode<T>* get_head() { return head; }
    const ListNode<T>* get_head() const { return head; }

    T* find(const T& value) {
        ListNode<T>* current = head;
        while (current != 0) {
            if (current->data == value) return &current->data;
            current = current->next;
        }
        return 0;
    }
};
