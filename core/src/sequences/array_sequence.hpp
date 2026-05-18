#pragma once

#include "../interfaces/sequence.hpp"
#include "../memory/dynamic_array.hpp"
#include <stdexcept>

template <class T>
class ArraySequence : public Sequence<T> {
protected:
    DynamicArray<T> *items; // protected so that heirs can copy it

    // method that determines which object to mutate
    virtual ArraySequence<T>* get_instance() = 0;

public:
    // constructors
    ArraySequence();
    explicit ArraySequence(int size);
    ArraySequence(T *items, int count);
    ArraySequence(const ArraySequence<T> &other);
    ArraySequence(ArraySequence<T> &&other) noexcept;
    // cross-constructor from any collection
    explicit ArraySequence(const ICollection<T> &collection);

    ~ArraySequence() override;

    // iterators
    IEnumerator<T>* get_enumerator() const override;

    class CppIterator {
    private:
        const ArraySequence<T>* seq;
        int current_index;

    public:
        CppIterator(const ArraySequence<T>* sequence, int index) : seq(sequence), current_index(index) {}

        const T& operator*() const {
            return seq->get(current_index);
        }
        CppIterator& operator++() {
            current_index++;
            return *this;
        }
        bool operator!=(const CppIterator& other) const {
            return current_index != other.current_index;
        }
    };

    // methods for using for (auto x : array_seq)
    CppIterator begin() const {
        return CppIterator(this, 0);
    }
    CppIterator end() const {
        return CppIterator(this, this->get_length());
    }

    // operators=
    // copy assignment operator
    ArraySequence<T>& operator=(const ArraySequence<T> &other);
    // move assignment operator
    ArraySequence<T>& operator=(ArraySequence<T> &&other) noexcept;

    // fabric methods
    Sequence<T>* create_empty() const override = 0;
    Sequence<T>* clone() const override = 0;

    // getters
    const T& get(int index) const override;
    [[nodiscard]] int get_length() const override;

    // modifying operations
    Sequence<T>* append(const T &item) override;
    Sequence<T>* prepend(const T &item) override;
    Sequence<T>* insert_at(const T &item, int index) override;
    Sequence<T>* remove_at(int index) override;
};

#include "array_sequence.tpp"