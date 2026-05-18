#pragma once

#include "Generator.hpp"
#include "sequence.hpp" 
#include <stdexcept>

template <typename T>
class FilterGenerator : public Generator<T> {
private:
    const Sequence<T>* source_sequence;
    bool (*predicate)(const T&);
    int current_index = 0; 

public:
    FilterGenerator(const Sequence<T>* source, bool (*pred)(const T&)) 
        : source_sequence(source), predicate(pred) {}

    Option<T> TryGetNext() override {
        while (true) {
            Option<T> next_opt;
            
            try {
                // Пытаемся получить элемент из родительского списка
                // Он сам решит отдать из кэша или сгенерировать новый
                T val = source_sequence->get(current_index++);
                next_opt = Option<T>(val); 
            } 
            catch (const std::out_of_range&) {
                // Если родительский список кончился, он бросит исключение
                return Option<T>();
            }
            
            if (predicate(next_opt.unwrap())) {
                return next_opt;
            }
        }
    }

    Cardinal get_cardinality() const override {
        // Вычислить точный размер отфильтрованного ленивого списка невозможно 
        // без его полной материализации (что приведет к зависанию на бесконечности)
        // Поэтому возвращаем Infinity
        return Cardinal::Infinity();
    }
};