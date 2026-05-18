#pragma once

#include "../types/exceptions.hpp"
#include <stdexcept>

template <class T>
class DynamicArray {
public:
    // constructors
    DynamicArray();
    explicit DynamicArray(int size);
    DynamicArray(T *items, int count);
    DynamicArray(const DynamicArray<T> &other);
    DynamicArray(DynamicArray<T> &&other) noexcept;
    ~DynamicArray();

    // operators=
    // copy assignment operator (strong exception guarantee)
    DynamicArray<T>& operator=(const DynamicArray<T> &other);
    // move assignment operator
    DynamicArray<T>& operator=(DynamicArray<T> &&other) noexcept;

    // getters
    const T& get(int index) const;
    [[nodiscard]] int get_size() const;
    [[nodiscard]] int get_capacity() const;

    // operation
    void set(int index, const T &value);
    void remove_at(int index);
    void resize(int new_size);

private:
    T *items;
    int size;
    int capacity;
};

#include "dynamic_array.tpp"