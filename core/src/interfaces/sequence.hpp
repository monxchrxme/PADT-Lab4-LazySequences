#pragma once

#include "icollection.hpp"
#include "ienumerable.hpp"
#include "isequence_builder.hpp"
#include "../types/exceptions.hpp"
#include "../types/option.hpp"

template <class T>
class Sequence : public ICollection<T>, public IEnumerable<T> {
public:
    virtual ~Sequence() = default;

    IEnumerator<T>* get_enumerator() const override = 0;

    // fabric methods (virtual constructors)
    // create an empty sequence of the same type (array or list)
    Sequence<T>* create_empty() const override = 0;
    // create an exact copy of the current sequence
    Sequence<T>* clone() const override = 0;

    // getters
    virtual const T& get_first() const;
    virtual const T& get_last() const;

    virtual Sequence<T>* get_subsequence(int start_index, int end_index) const;
    virtual Sequence<T>* concat(Sequence<T> *list) const;
    virtual Sequence<T>* slice(int index, int count, const Sequence<T>* elements = nullptr) const;

    // modifying operations
    virtual Sequence<T>* append(const T &item) = 0;
    virtual Sequence<T>* prepend(const T &item) = 0;
    virtual Sequence<T>* insert_at(const T &item, int index) = 0;
    virtual Sequence<T>* remove_at(int index) = 0;

    // where / reduce operations
    virtual Sequence<T>* where(bool (*predicate)(const T&)) const;
    virtual T reduce(T (*reducer)(const T&, const T&), const T &initial_value) const;

    // Try-semantics
    Option<T> try_get_first() const;
    Option<T> try_get_last() const;
    Option<T> try_get(int index) const;

    // overloading operator []
    // operator[] (read-only, guarantees immutability safety)
    const T& operator[](int index) const {
        return this->get(index);
    }

    // fabric method: each class knows which builder it needs
    virtual ISequenceBuilder<T>* create_builder() const = 0;
};

#include "sequence.tpp"