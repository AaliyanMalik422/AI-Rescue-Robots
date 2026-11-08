#pragma once

#include <cstddef>
#include <memory>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

// A small dynamically-resized array implemented without STL containers.
// Only the first size() slots contain live T objects; remaining capacity is
// raw storage. This allows pop_back() and clear() to destroy elements rather
// than merely assigning them a default value.
template <typename T>
class DynamicArray {
private:
    using Allocator = std::allocator<T>;
    using Traits = std::allocator_traits<Allocator>;

    Allocator allocator;
    T* data;
    std::size_t cap;
    std::size_t len;

    void destroy_range(T* ptr, std::size_t first, std::size_t last) noexcept {
        while (last > first) {
            --last;
            Traits::destroy(allocator, ptr + last);
        }
    }

    void release_storage() noexcept {
        if (data != nullptr) {
            destroy_range(data, 0, len);
            Traits::deallocate(allocator, data, cap);
        }
        data = nullptr;
        cap = 0;
        len = 0;
    }

    void reserve(std::size_t new_capacity) {
        if (new_capacity <= cap) return;

        T* new_data = Traits::allocate(allocator, new_capacity);
        std::size_t constructed = 0;
        try {
            for (; constructed < len; ++constructed) {
                // Copy when move may throw and copying is available. For
                // move-only types with throwing moves, the usual basic
                // guarantee applies if a move itself throws.
                Traits::construct(allocator, new_data + constructed,
                                 std::move_if_noexcept(data[constructed]));
            }
        } catch (...) {
            destroy_range(new_data, 0, constructed);
            Traits::deallocate(allocator, new_data, new_capacity);
            throw;
        }

        destroy_range(data, 0, len);
        if (data != nullptr) Traits::deallocate(allocator, data, cap);
        data = new_data;
        cap = new_capacity;
    }

    bool aliases_element(const T& value) const noexcept {
        for (std::size_t i = 0; i < len; ++i) {
            if (std::addressof(data[i]) == std::addressof(value)) return true;
        }
        return false;
    }

public:
    DynamicArray() noexcept : data(nullptr), cap(0), len(0) {}

    ~DynamicArray() { release_storage(); }

    DynamicArray(const DynamicArray& other) : data(nullptr), cap(0), len(0) {
        if (other.cap == 0) return;

        T* new_data = Traits::allocate(allocator, other.cap);
        std::size_t constructed = 0;
        try {
            for (; constructed < other.len; ++constructed) {
                Traits::construct(allocator, new_data + constructed,
                                 other.data[constructed]);
            }
        } catch (...) {
            destroy_range(new_data, 0, constructed);
            Traits::deallocate(allocator, new_data, other.cap);
            throw;
        }

        data = new_data;
        cap = other.cap;
        len = other.len;
    }

    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            DynamicArray copy(other);
            swap(copy);
        }
        return *this;
    }

    DynamicArray(DynamicArray&& other) noexcept
        : data(other.data), cap(other.cap), len(other.len) {
        other.data = nullptr;
        other.cap = 0;
        other.len = 0;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            release_storage();
            data = other.data;
            cap = other.cap;
            len = other.len;
            other.data = nullptr;
            other.cap = 0;
            other.len = 0;
        }
        return *this;
    }

    void swap(DynamicArray& other) noexcept {
        using std::swap;
        swap(data, other.data);
        swap(cap, other.cap);
        swap(len, other.len);
    }

    void push_back(const T& value) {
        // Copy first so push_back(array[i]) remains valid if reserve reallocates.
        T stable_copy(value);
        if (len == cap) {
            if (cap > std::numeric_limits<std::size_t>::max() / 2) {
                throw std::length_error("DynamicArray capacity overflow");
            }
            reserve(cap == 0 ? 2 : cap * 2);
        }
        Traits::construct(allocator, data + len, std::move_if_noexcept(stable_copy));
        ++len;
    }

    void push_back(T&& value) {
        // Materialize the argument before reserve, so an rvalue alias cannot
        // become a dangling reference when the backing storage is replaced.
        T stable_value(std::move(value));
        if (len == cap) {
            if (cap > std::numeric_limits<std::size_t>::max() / 2) {
                throw std::length_error("DynamicArray capacity overflow");
            }
            reserve(cap == 0 ? 2 : cap * 2);
        }
        Traits::construct(allocator, data + len, std::move(stable_value));
        ++len;
    }

    void pop_back() noexcept {
        if (len == 0) return;
        --len;
        Traits::destroy(allocator, data + len);
    }

    void clear() noexcept {
        destroy_range(data, 0, len);
        len = 0;
    }

    std::size_t size() const noexcept { return len; }
    std::size_t capacity() const noexcept { return cap; }
    bool empty() const noexcept { return len == 0; }

    T& operator[](std::size_t index) {
        if (index >= len) throw std::out_of_range("DynamicArray index out of bounds");
        return data[index];
    }

    const T& operator[](std::size_t index) const {
        if (index >= len) throw std::out_of_range("DynamicArray index out of bounds");
        return data[index];
    }
};

template <typename T>
struct Node {
    T data;
    Node* next;

    explicit Node(const T& value) : data(value), next(nullptr) {}
    explicit Node(T&& value) : data(std::move(value)), next(nullptr) {}
};

// Singly linked list with explicit ownership of every node.
template <typename T>
class LinkedList {
private:
    Node<T>* head;
    Node<T>* tail;
    std::size_t count;

public:
    LinkedList() noexcept : head(nullptr), tail(nullptr), count(0) {}

    ~LinkedList() { clear(); }

    LinkedList(const LinkedList& other) : head(nullptr), tail(nullptr), count(0) {
        try {
            for (Node<T>* current = other.head; current != nullptr; current = current->next) {
                push_back(current->data);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            LinkedList copy(other);
            swap(copy);
        }
        return *this;
    }

    LinkedList(LinkedList&& other) noexcept
        : head(other.head), tail(other.tail), count(other.count) {
        other.head = nullptr;
        other.tail = nullptr;
        other.count = 0;
    }

    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head = other.head;
            tail = other.tail;
            count = other.count;
            other.head = nullptr;
            other.tail = nullptr;
            other.count = 0;
        }
        return *this;
    }

    void swap(LinkedList& other) noexcept {
        using std::swap;
        swap(head, other.head);
        swap(tail, other.tail);
        swap(count, other.count);
    }

    void clear() noexcept {
        Node<T>* current = head;
        while (current != nullptr) {
            Node<T>* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
        tail = nullptr;
        count = 0;
    }

    void push_front(const T& value) {
        Node<T>* node = new Node<T>(value);
        node->next = head;
        head = node;
        if (tail == nullptr) tail = node;
        ++count;
    }

    void push_front(T&& value) {
        Node<T>* node = new Node<T>(std::move(value));
        node->next = head;
        head = node;
        if (tail == nullptr) tail = node;
        ++count;
    }

    void push_back(const T& value) {
        Node<T>* node = new Node<T>(value);
        if (tail == nullptr) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        ++count;
    }

    void push_back(T&& value) {
        Node<T>* node = new Node<T>(std::move(value));
        if (tail == nullptr) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        ++count;
    }

    void pop_front() noexcept {
        if (head == nullptr) return;
        Node<T>* removed = head;
        head = head->next;
        delete removed;
        --count;
        if (head == nullptr) tail = nullptr;
    }

    void pop_back() noexcept {
        if (head == nullptr) return;
        if (head == tail) {
            delete head;
            head = tail = nullptr;
            count = 0;
            return;
        }

        Node<T>* previous = head;
        while (previous->next != tail) previous = previous->next;
        delete tail;
        tail = previous;
        tail->next = nullptr;
        --count;
    }

    std::size_t size() const noexcept { return count; }
    bool empty() const noexcept { return count == 0; }

    Node<T>* get_head() noexcept { return head; }
    const Node<T>* get_head() const noexcept { return head; }

    T* find(const T& value) {
        for (Node<T>* current = head; current != nullptr; current = current->next) {
            if (current->data == value) return &current->data;
        }
        return nullptr;
    }

    const T* find(const T& value) const {
        for (const Node<T>* current = head; current != nullptr; current = current->next) {
            if (current->data == value) return &current->data;
        }
        return nullptr;
    }
};
