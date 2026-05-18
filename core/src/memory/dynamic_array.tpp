#pragma once
#include "dynamic_array.hpp"

// constructors

template <class T>
DynamicArray<T>::DynamicArray() : items{nullptr}, size{0}, capacity{0} {}

template <class T>
DynamicArray<T>::DynamicArray(int size) : size{size}, capacity{size} {
    if (size < 0) {
        throw std::invalid_argument("DynamicArray: size cannot be negative");
    }
    this->items = (size > 0) ? new T[size]{} : nullptr;
}

template <class T>
DynamicArray<T>::DynamicArray(T *items, int count) : size{count}, capacity{count} {
    if (count < 0) {
        throw std::invalid_argument("DynamicArray: count cannot be negative");
    }
    if (items == nullptr && count > 0) {
        throw std::invalid_argument("DynamicArray: source items pointer is nullptr");
    }

    this->items = (count > 0) ? new T[count]{} : nullptr;

    if (this->items != nullptr) {
        try {
            for (int i = 0; i < count; ++i) {
                this->items[i] = items[i];
            }
        } catch (...) {
            delete []this->items;
            throw;
        }
    }
}

template <class T>
DynamicArray<T>::DynamicArray(const DynamicArray<T> &other) : size{other.size}, capacity{other.capacity} {
    this->items = (this->capacity > 0) ? new T[this->capacity]{} : nullptr;

    if (this->items != nullptr) {
        try {
            for (int i = 0; i < this->size; ++i) {
                this->items[i] = other.items[i];
            }
        } catch (...) {
            delete []this->items;
            throw;
        }
    }
}

template <class T>
DynamicArray<T>::DynamicArray(DynamicArray<T> &&other) noexcept
    : items{other.items}, size{other.size}, capacity{other.capacity} {
    other.items = nullptr;
    other.size = 0;
    other.capacity = 0;
}

template <class T>
DynamicArray<T>::~DynamicArray() {
    delete []this->items;
}

// operators=

template <class T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T> &other) {
    if (this == &other) {
        return *this;
    }

    T *new_items = (other.capacity > 0) ? new T[other.capacity]{} : nullptr;

    if (new_items != nullptr) {
        try {
            for (int i = 0; i < other.size; ++i) {
                new_items[i] = other.items[i];
            }
        } catch (...) {
            delete []new_items;
            throw;
        }
    }

    delete []this->items;
    this->items = new_items;
    this->size = other.size;
    this->capacity = other.capacity;

    return *this;
}

template <class T>
DynamicArray<T>& DynamicArray<T>::operator=(DynamicArray<T> &&other) noexcept {
    if (this == &other) {
        return *this;
}

    delete []this->items;

    this->items = other.items;
    this->size = other.size;
    this->capacity = other.capacity;

    other.items = nullptr;
    other.size = 0;
    other.capacity = 0;

    return *this;
}

// getters

template <class T>
const T& DynamicArray<T>::get(int index) const {
    if (index < 0 || index >= this->size) {
        throw IndexOutOfRange("DynamicArray::get: Index out of range");
    }
    return this->items[index];
}

template <class T>
int DynamicArray<T>::get_size() const {
    return this->size;
}

template <class T>
int DynamicArray<T>::get_capacity() const {
    return this->capacity;
}

// operations

template <class T>
void DynamicArray<T>::set(int index, const T &value) {
    if (index < 0 || index >= this->size) {
        throw IndexOutOfRange("DynamicArray::set: Index out of range");
    }
    this->items[index] = value;
}

template <class T>
void DynamicArray<T>::remove_at(int index) {
    if (index < 0 || index >= this->size) {
        throw IndexOutOfRange("DynamicArray::remove_at: Index out of range");
    }

    for (int i = index; i + 1 < this->size; ++i) {
        this->items[i] = this->items[i + 1];
    }

    this->items[this->size - 1] = T(); // set "clear" value instead unavailable value out of size to save memory
    this->size--;
}

template <class T>
void DynamicArray<T>::resize(int new_size) {
    if (new_size < 0) {
        throw std::invalid_argument("DynamicArray::resize: new_size cannot be negative");
    }

    if (new_size == 0) {
        delete []this->items;
        this->items = nullptr;
        this->size = 0;
        this->capacity = 0;
        return;
    }

    if (new_size <= this->capacity) {
        for (int i = new_size; i < this->size; ++i) {
            this->items[i] = T(); // set "clear" value instead unavailable value out of size to save memory
        }

        this->size = new_size;
        return;
    }

    int new_capacity = (new_size > this->capacity * 2) ? new_size : this->capacity * 2;
    T *new_items = new T[new_capacity]{};

    try {
        for (int i = 0; i < this->size; ++i) {
            new_items[i] = this->items[i];
        }
    } catch (...) {
        delete []new_items;
        throw;
    }

    delete []this->items;
    this->items = new_items;
    this->size = new_size;
    this->capacity = new_capacity;
}