#pragma once

#include "IAllocatedObject.hpp"
#include "option.hpp" 
#include "Cardinal.hpp"

template <typename T>
class Generator : public IAllocatedObject {
public:
    virtual ~Generator() = default;

    // Возвращает Option с элементом, либо пустой Option, если достигнут конец
    virtual Option<T> TryGetNext() = 0;

    virtual Cardinal get_cardinality() const = 0;
};