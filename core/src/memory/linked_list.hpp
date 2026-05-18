#pragma once

#include "../types/exceptions.hpp"
#include <stdexcept>

template <class T>
class LinkedList {
public:
    struct Node {
        T data;
        Node *next;
        Node *prev;

        // node constructor
        explicit Node(const T &data) : data(data), next(nullptr), prev(nullptr) {}
    };

    // method for start iterating
    Node* get_head_node() const {
        return head;
    }

    // constructors
    LinkedList();
    LinkedList(T *items, int count);
    LinkedList(const LinkedList<T> &other);
    LinkedList(LinkedList<T> &&other) noexcept;
    ~LinkedList();

    // operators=
    // copy assignment operator (strong exception guarantee)
    LinkedList<T>& operator=(const LinkedList<T> &other);
    // move assignment operator
    LinkedList<T>& operator=(LinkedList<T> &&other) noexcept;

    // getters
    const T& get_first() const;
    const T& get_last() const;
    const T& get(int index) const;
    [[nodiscard]] int get_length() const;

    LinkedList<T>* get_sub_list(int start_index, int end_index) const;

    // operations
    void append(const T &item);
    void prepend(const T &item);
    void insert_at(const T &item, int index);
    void remove_at(int index);

    LinkedList<T>* concat(LinkedList<T> *list) const;

private:
    Node *head;
    Node *tail;
    int size;

    // auxiliary method to clear the whole list
    void clear();
};

#include "linked_list.tpp"