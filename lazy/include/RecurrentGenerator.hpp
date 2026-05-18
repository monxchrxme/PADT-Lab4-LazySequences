#pragma once

#include "Generator.hpp"
#include "sequence.hpp"
#include "mutable_array_sequence.hpp"

template <typename T>
class RecurrentGenerator : public Generator<T> {
private:
    // Указатель на функцию-правило
    // Принимает всё текущее окно (Sequence) и возвращает новый элемент
    Option<T> (*rule)(const Sequence<T>&);
    
    MutableArraySequence<T> window;
    
    int window_size;
    int initial_yielded = 0; // Счетчик для выдачи стартовых элементов
    Cardinal cardinality;

public:
    // Конструктор принимает стартовые элементы окна и само правило
    RecurrentGenerator(const Sequence<T>* initial_elements, 
                       Option<T> (*r)(const Sequence<T>&), 
                       Cardinal card = Cardinal::Infinity())
        : rule(r), cardinality(card) 
    {
        
        for (int i = 0; i < initial_elements->get_length(); ++i) {
            window.append(initial_elements->get(i));
        }
        window_size = window.get_length();
    }

    Option<T> TryGetNext() override {
        // Отдаем стартовые элементы 
        if (initial_yielded < window_size) {
            Option<T> val(window.get(initial_yielded));
            initial_yielded++;
            return val;
        }

        // Применяем правило к текущему окно после (после конца стартовых эл-тов) 
        Option<T> next_opt = rule(window);

        // Если правило смогло сгенерировать элемент то сдвигаем окно
        if (next_opt.is_some()) {
            window.remove_at(0); 
            window.append(next_opt.unwrap());
        }

        return next_opt;
    }

    Cardinal get_cardinality() const override {
        return cardinality;
    }
};