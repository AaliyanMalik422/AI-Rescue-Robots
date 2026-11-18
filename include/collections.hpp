#pragma once

// ------------------------------------------------------------
// MEMBER 2 - Custom collections
// ------------------------------------------------------------
// DynamicArray<T>
// push_back amortised O(1), index O(1), remove_at O(n).
// -------------------------------------------------------------
template <typename T>
class DynamicArray {//1d array
private:
    T* data;
    int len;
    int curr_cap;

    bool grow() {//resize
        int new_cap;
        if (curr_cap == 0) {

            new_cap = 4;
        }
        else if (curr_cap > 1073741823)// doubling would overflow int
            return false;
        
        else 
            new_cap = curr_cap * 2;
        T* new_block = new T[new_cap];

        for (int i = 0; i < length; ++i) 
            new_block[i] = data[i];

        delete[] data;
        data = new_block;
        curr_cap = new_cap;
        return true;
    }

public:
    DynamicArray() : data(0), len(0), curr_cap(0) {}//defuault constructor

    DynamicArray(const DynamicArray<T>& other)//deep copy constructor
        : data(0), len(0), curr_cap(0) {

        if (other.curr_cap > 0) {//if other has some data
            data = new T[other.curr_cap];

            for (int i = 0; i < other.len; ++i)
                data[i] = other.data[i];

            len = other.len;
            curr_cap = other.curr_cap;
        }
    }

    DynamicArray<T>& operator=(const DynamicArray<T>& other) {//assingment constructor

        if (this == &other) 
            return *this;

        T* new_block = 0;

        if (other.curr_cap > 0) {
            new_block = new T[other.curr_cap];

            for (int i = 0; i < other.len; ++i)
                new_block[i] = other.data[i];
        }
        delete[] data;
        data = new_block;

        len = other.len;
        curr_cap = other.curr_cap;

        return *this;
    }

    ~DynamicArray() { delete[] data; }

    bool push_back(const T& val) {

       

        if (len == curr_cap && !grow()) //if size is full and cant resise 
            return false;

        data[len] = val;
        len++;

        return true;
    }

    bool insert_at(int idx, const T& val) {
      
        if (idx<0 or idx > len)
            return false;
        
        if (len == curr_cap && !grow()) 
            return false;

        for (int i = len;i > idx;i--) {//shifting right til index 
            data[i]=data[i-1]
        }
        data[idx] = val;
        len++;
        return true;
    }

    bool pop_back() {
        if (len == 0) 
            return false;
        len--;
        data[len] = T();

        return true;
    }

    bool remove_at(int idx) {

        if (idx < 0 || idx >= len) 
            return false;//invalid index

        for (int i = idx; i < len - 1; ++i) //shifting
            data[i] = data[i + 1];

        len--;
        data[len] = T();
        return true;
    }

    void clear() {
        for (int i = 0; i < len; ++i) data[i] = T();
        len = 0;
    }

    int size() const { return len; }
    int capacity() const { return curr_cap; }
    bool empty() const { return len == 0; }

    T& operator[](int idx) { return data[idx]; }
    const T& operator[](int idx) const { return data[idx]; }

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
// Singly linked list
// push_front/push_back O(1), pop_back O(n), find O(n).
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
    int len;

public:
    LinkedList() : head(0), tail(0), len(0) {}//defualt constructor

    LinkedList(const LinkedList<T>& other) : head(0), tail(0), len(0) {//deep copy

        const ListNode<T>* node = other.head;
        while (node != 0) {
            push_back(node->data);
            node = node->next;

    }

    LinkedList<T>& operator=(const LinkedList<T>& other) {//deep copy-assingment constructor
        
        if (this == &other) 
            return *this;
        clear();

        const ListNode<T>* node = other.head;

        while (node != 0) {
            push_back(node->data);
            node = node->next;
        }
        return *this;

    }
    ~LinkedList() { clear(); }

    bool push_front(const T& value) {

        ListNode<T>* nn = new ListNode<T>(value);

        nn->next = head;
        head = nn;

        if (tail == 0) 
            tail = nn;

        len++;
        return true;
    }
    bool push_back(const T& value) { 

        ListNode<T>* nn = new ListNode<T>(value);

        if (tail == 0) {
            head = nn;
            tail = nn;
        }
        else {
            tail->next = nn;
            tail = nn;
        }
        len++;
        return true;
    }

    bool pop_front() { 

        if (head == 0) 
            return false;

        ListNode<T>* tmp = head;
        head = head->next;
        delete tmp;

        len--;

        if (head == 0) 
            tail = 0;
        return true;
    }

    bool pop_back() {
        if (head == 0) 
            return false;

        if (head == tail) {
            delete head;
            head = 0;
            tail = 0;
            len = 0;
            return true;
        }

        ListNode<T>* tmp = head;

        while (tmp->next != tail) 
            tmp = tmp->next;

        delete tail;
        tail = tmp;
        tail->next = 0;
        len--;
        return true;
    }

    void clear() {
    
        while (head != 0) 
            pop_front();
        tail = 0;
        len = 0;
    }

    T* find(const T& value) { 
    
        ListNode<T>* node = head;
        while (node != 0) {
            if (node->data == value) 
                return &node->data;
            node = node->next;
        }
        return 0;
    }

    int size() const { return length; }
    bool empty() const { return length == 0; }
    ListNode<T>* get_head() { return head; }
    const ListNode<T>* get_head() const { return head; }
};
