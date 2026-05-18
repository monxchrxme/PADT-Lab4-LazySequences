#pragma once
#include "ienumerator.hpp"

// interface of iterable collection
template <class T>
class IEnumerable {
public:
    // factory method returning an iterator for traversing the collection
    // return a pointer, as IEnumerator is an abstract class (to prevent Object Slicing)
    virtual IEnumerator<T>* get_enumerator() const = 0;

    virtual ~IEnumerable() = default;
};
