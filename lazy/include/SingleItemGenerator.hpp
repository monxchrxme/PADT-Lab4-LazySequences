#pragma once
#include "Generator.hpp"

template <typename T>
class SingleItemGenerator : public Generator<T> {
private:
    T item;
    bool yielded = false;

public:
    SingleItemGenerator(const T& val) : item(val) {}

    Option<T> TryGetNext() override {
        if (!yielded) {
            yielded = true;
            return Option<T>(item);
        }
        return Option<T>();
    }

    Cardinal get_cardinality() const override {
        return Cardinal(1);
    }
};