#pragma once
#include "../types/exceptions.hpp"

// abstract class for enumerator (interface enumerator)
template <class T>
class IEnumerator {
public:
    // move to next element
    // returns true if the element exists, and false if we have reached the end of the collection
    virtual bool move_next() = 0;

    // return the current element
    // if called before the first "move_next()" or after the end of the collection, it throws an error
    virtual const T& get_current() const = 0;

    // resets the iterator to its initial state (before the first element)
    virtual void reset() = 0;

    virtual ~IEnumerator() = default;
};