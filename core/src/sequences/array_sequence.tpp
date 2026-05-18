#pragma once
#include "array_sequence.hpp"

// Nested Class iterator for an array

template <class T>
class ArrayEnumerator : public IEnumerator<T> {
private:
    const ArraySequence<T>* sequence;
    int current_index;

public:
    // -1 indicates the state "before the first element"
    explicit ArrayEnumerator(const ArraySequence<T>* seq) : sequence(seq), current_index(-1) {}

    bool move_next() override {
        if (current_index + 1 < sequence->get_length()) {
            current_index++;
            return true;
        } else {
            current_index = sequence->get_length();
            return false;
        }
    }

    const T& get_current() const override {
        if (current_index < 0 || current_index >= sequence->get_length()) {
            throw IndexOutOfRange("IEnumerator: Invalid state (call move_next() first)");
        }
        return sequence->get(current_index);
    }

    void reset() override {
        current_index = -1;
    }
};

// realization of the get_enumerator method
template <class T>
IEnumerator<T>* ArraySequence<T>::get_enumerator() const {
    return new ArrayEnumerator<T>(this);
}

// constructors

template <class T>
ArraySequence<T>::ArraySequence() {
    this->items = new DynamicArray<T>();
}

template <class T>
ArraySequence<T>::ArraySequence(int size) {
    this->items = new DynamicArray<T>(size);
}

template <class T>
ArraySequence<T>::ArraySequence(T *items, int count) {
    this->items = new DynamicArray<T>(items, count);
}

template <class T>
ArraySequence<T>::ArraySequence(const ArraySequence<T> &other) {
    this->items = new DynamicArray<T>(*(other.items));
}

template <class T>
ArraySequence<T>::ArraySequence(ArraySequence<T> &&other) noexcept {
    this->items = other.items;
    other.items = nullptr;
}

template <class T>
ArraySequence<T>::ArraySequence(const ICollection<T> &collection) {
    int length = collection.get_length();
    // OPTIMIZATION: we know the size, so we allocate memory in one piece,
    // instead of doing a slow append() in a cycle
    this->items = new DynamicArray<T>(length);

    for (int i = 0; i < length; ++i) {
        this->items->set(i, collection.get(i));
    }
}

template <class T>
ArraySequence<T>::~ArraySequence() {
    // here we delete a single DynamicArray object, not an array,
    // so we use the usual delete, not delete []
    delete this->items;
}

// operators=

template <class T>
ArraySequence<T>& ArraySequence<T>::operator=(const ArraySequence<T> &other) {
    if (this == &other) {
        return *this;
    }
    // delegate copying to the overloaded operator = inside DynamicArray
    *(this->items) = *(other.items);
    return *this;
}

template <class T>
ArraySequence<T>& ArraySequence<T>::operator=(ArraySequence<T> &&other) noexcept {
    if (this == &other) {
        return *this;
    }
    delete this->items;
    this->items = other.items;
    other.items = nullptr;
    return *this;
}

// getters

template <class T>
const T& ArraySequence<T>::get(int index) const {
    return this->items->get(index);
}

template <class T>
int ArraySequence<T>::get_length() const {
    return this->items->get_size();
}

// modifying operations

template <class T>
Sequence<T>* ArraySequence<T>::append(const T &item) {
    // if Mutable, target == this
    // if Immutable, target == new clone on the heap
    ArraySequence<T>* target = this->get_instance();

    int old_size = target->get_length();
    target->items->resize(old_size + 1);
    target->items->set(old_size, item);

    return target;
}

template <class T>
Sequence<T>* ArraySequence<T>::insert_at(const T &item, int index) {
    int old_size = this->get_length();

    // OPTIMIZATION: Check boundaries BEFORE cloning
    if (index < 0 || index > old_size) {
        throw IndexOutOfRange("ArraySequence::insert_at: Index out of range");
    }

    ArraySequence<T>* target = this->get_instance();

    target->items->resize(old_size + 1);

    for (int i = old_size; i > index; --i) {
        target->items->set(i, target->items->get(i - 1));
    }

    target->items->set(index, item);

    return target;
}

template <class T>
Sequence<T>* ArraySequence<T>::prepend(const T &item) {
    return this->insert_at(item, 0); // reusing the insert_at logic
}

template <class T>
Sequence<T>* ArraySequence<T>::remove_at(int index) {
    if (index < 0 || index >= this->get_length()) {
        throw IndexOutOfRange("ArraySequence::remove_at: Index out of range");
    }

    ArraySequence<T>* target = this->get_instance();
    target->items->remove_at(index);

    return target;
}
