#pragma once
#include "isequence_builder.hpp"
#include "CollectionGenerator.hpp"
#include "LazyContext.hpp"

template <typename T> class LazySequence; 

template <typename T>
class LazySequenceBuilder : public ISequenceBuilder<T>, public IAllocatedObject {
private:
    LazyContext* context;
    MutableArraySequence<T> buffer;

public:
    LazySequenceBuilder(LazyContext* ctx) : context(ctx) {}

    void append(const T& item) override {
        buffer.append(item);
    }

    Sequence<T>* build() override;
};