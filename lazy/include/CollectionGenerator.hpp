#pragma once
#include "Generator.hpp"
#include "mutable_array_sequence.hpp"

template <typename T>
class CollectionGenerator : public Generator<T> {
private:
    MutableArraySequence<T> items;
    int current_index = 0;

public:
    // Копируем стартовые данные внутрь генератора
    CollectionGenerator(const MutableArraySequence<T>& initial_items) 
        : items(initial_items) {}

    Option<T> TryGetNext() override {
        // Если элементы еще остались, выдаем и сдвигаем индекс
        if (current_index < items.get_length()) {
            return Option<T>(items.get(current_index++));
        }
        
        // Если закончились генератор исчерпан
        return Option<T>();
    }

    Cardinal get_cardinality() const override {
        // Длина конечна и известна заранее
        return Cardinal(items.get_length());
    }
};