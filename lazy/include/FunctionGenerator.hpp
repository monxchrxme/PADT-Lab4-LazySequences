#pragma once

#include "Generator.hpp"

template <typename T>
class FunctionGenerator : public Generator<T> {
private:
    // Храним сырой указатель на функцию 
    // Функция принимает int (индекс) и возвращает Option<T>
    Option<T> (*generate_func)(int);
    int current_index = 0;
    Cardinal cardinality;

public:
    // По умолчанию считаем такие генераторы бесконечными
    explicit FunctionGenerator(Option<T> (*func)(int), Cardinal card = Cardinal::Infinity()) 
        : generate_func(func), cardinality(card) {}

    Option<T> TryGetNext() override {
        // Если генератор конечен и мы уже выдали нужное количество элементов принудительный стоп
        if (!cardinality.is_infinite() && current_index >= cardinality.get_value()) {
            return Option<T>(); 
        }

        Option<T> result = generate_func(current_index);
        if (result.is_some()) { 
            current_index++;
        }
        return result;
    }

    Cardinal get_cardinality() const override {
        return cardinality;
    }
};