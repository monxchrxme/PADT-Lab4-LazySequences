#pragma once
#include "linked_list.hpp"

// auxiliary methods

template <class T>
void LinkedList<T>::clear() {
    Node *current = this->head;
    while (current != nullptr) {
        Node *next_node = current->next;
        delete current;
        current = next_node;
    }
    this->head = nullptr;
    this->tail = nullptr;
    this->size = 0;
}

// constructors

template <class T>
LinkedList<T>::LinkedList() : head{nullptr}, tail{nullptr}, size{0} {}

template <class T>
LinkedList<T>::LinkedList(T *items, int count) : LinkedList() {
    if (count < 0) {
        throw std::invalid_argument("LinkedList: count cannot be negative");
    }
    if (items == nullptr && count > 0) {
        throw std::invalid_argument("LinkedList: source items pointer is nullptr");
    }

    try {
        for (int i = 0; i < count; ++i) {
            this->append(items[i]);
        }
    } catch (...) {
        this->clear();
        throw;
    }
}

template <class T>
LinkedList<T>::LinkedList(const LinkedList<T> &other) : LinkedList() {
    try {
        Node *current = other.head;
        while (current != nullptr) {
            this->append(current->data);
            current = current->next;
        }
    } catch (...) {
        this->clear();
        throw;
    }
}

template <class T>
LinkedList<T>::LinkedList(LinkedList<T> &&other) noexcept
    : head{other.head}, tail{other.tail}, size{other.size} {
    other.head = nullptr;
    other.tail = nullptr;
    other.size = 0;
}

template <class T>
LinkedList<T>::~LinkedList() {
    this->clear();
}

// operators=

template <class T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T> &other) {
    if (this == &other) {
        return *this;
    }

    // copy-and-Swap idiom (if we get exception in the middle of the process, other and this lists will be ok)
    // 1. make temp list and try to copy data to it
    LinkedList<T> temp;
    Node *current = other.head;
    while (current != nullptr) {
        temp.append(current->data); // if we get an exception here, temp will use its destructor and clear nodes
        current = current->next;
    }

    // 2. clear old data
    this->clear();

    // 3. swap pointers
    this->head = temp.head;
    this->tail = temp.tail;
    this->size = temp.size;

    // 4. leave temp list free
    temp.head = nullptr;
    temp.tail = nullptr;
    temp.size = 0;

    return *this;
}

template <class T>
    LinkedList<T>& LinkedList<T>::operator=(LinkedList<T> &&other) noexcept {
    if (this == &other) {
        return *this;
}

    this->clear();

    this->head = other.head;
    this->tail = other.tail;
    this->size = other.size;

    other.head = nullptr;
    other.tail = nullptr;
    other.size = 0;

    return *this;
}

// getters

template <class T>
const T& LinkedList<T>::get_first() const {
    if (this->size == 0) {
        throw IndexOutOfRange("LinkedList::get_first: List is empty");
    }
    return this->head->data;
}

template <class T>
const T& LinkedList<T>::get_last() const {
    if (this->size == 0) {
        throw IndexOutOfRange("LinkedList::get_last: List is empty");
    }
    return this->tail->data;
}

template <class T>
const T& LinkedList<T>::get(int index) const {
    if (index < 0 || index >= this->size) {
        throw IndexOutOfRange("LinkedList::get: Index out of range");
    }

    // search optimization: if index in the second half, go from the tail
    if (index < this->size / 2) {
        Node *current = this->head;
        for (int i = 0; i < index; ++i) {
            current = current->next;
        }
        return current->data;
    } else {
        Node *current = this->tail;
        for (int i = this->size - 1; i > index; --i) {
            current = current->prev;
        }
        return current->data;
    }
}

template <class T>
int LinkedList<T>::get_length() const {
    return this->size;
}

template <class T>
LinkedList<T>* LinkedList<T>::get_sub_list(int start_index, int end_index) const {
    if (start_index < 0 || end_index < 0 || start_index >= this->size || end_index >= this->size || start_index > end_index) {
        throw IndexOutOfRange("LinkedList::get_sub_list: Invalid indices");
    }

    auto *sub_list = new LinkedList<T>();
    try {
        Node *current = this->head;
        for (int i = 0; i <= end_index; ++i) {
            if (i >= start_index) {
                sub_list->append(current->data);
            }
            current = current->next;
        }
    } catch (...) {
        delete sub_list;
        throw;
    }

    return sub_list;
}

// operations

template <class T>
void LinkedList<T>::append(const T &item) {
    Node *new_node = new Node(item); // we will get an exceptions if there is no memory

    if (this->size == 0) {
        this->head = new_node;
        this->tail = new_node;
    } else {
        this->tail->next = new_node;
        new_node->prev = this->tail;
        this->tail = new_node;
    }
    this->size++;
}

template <class T>
void LinkedList<T>::prepend(const T &item) {
    Node *new_node = new Node(item);

    if (this->size == 0) {
        this->head = new_node;
        this->tail = new_node;
    } else {
        new_node->next = this->head;
        this->head->prev = new_node;
        this->head = new_node;
    }
    this->size++;
}

template <class T>
void LinkedList<T>::insert_at(const T &item, int index) {
    if (index < 0 || index > this->size) {
        throw IndexOutOfRange("LinkedList::insert_at: Index out of range");
    }

    if (index == 0) {
        this->prepend(item);
        return;
    }
    if (index == this->size) {
        this->append(item);
        return;
    }

    Node *current = this->head;
    for (int i = 0; i < index; ++i) {
        current = current->next;
    }

    Node *new_node = new Node(item);
    new_node->prev = current->prev;
    new_node->next = current;

    current->prev->next = new_node;
    current->prev = new_node;

    this->size++;
}

template <class T>
void LinkedList<T>::remove_at(int index) {
    if (index < 0 || index >= this->size) {
        throw IndexOutOfRange("LinkedList::remove_at: Index out of range");
    }

    Node *to_delete = this->head;

    if (index == 0) {
        this->head = to_delete->next;
        if (this->head != nullptr) {
            this->head->prev = nullptr;
        } else {
            this->tail = nullptr;
        }
    } else if (index == this->size - 1) {
        to_delete = this->tail;
        this->tail = to_delete->prev;
        this->tail->next = nullptr;
    } else {
        for (int i = 0; i < index; ++i) {
            to_delete = to_delete->next;
        }
        to_delete->prev->next = to_delete->next;
        to_delete->next->prev = to_delete->prev;
    }

    delete to_delete; // delete node from memory, the type T destructor will be called automatically
    this->size--;
}

template <class T>
LinkedList<T>* LinkedList<T>::concat(LinkedList<T> *list) const {
    auto *new_list = new LinkedList<T>(*this); // secure copy via the constructor

    if (list != nullptr) {
        try {
            Node *current = list->head;
            while (current != nullptr) {
                new_list->append(current->data);
                current = current->next;
            }
        } catch (...) {
            delete new_list;
            throw;
        }
    }
    return new_list;
}