#pragma once

// forward declaration, to avoid a circular dependency
template <class T> class Sequence;

template <class T>
class ISequenceBuilder {
public:
    virtual ~ISequenceBuilder() = default;

    // quickly adds an element to the internal buffer
    virtual void append(const T& item) = 0;

    // "freezes" the result and returns the finished collection.
    virtual Sequence<T>* build() = 0;
};