#pragma once
#include "Generator.hpp"
#include "sequence.hpp"

template <typename T>
class SliceGenerator : public Generator<T> {
private:
    const Sequence<T>* source;
    int current_idx;
    int elements_left; // Если -1, значит лимита нет (бесконечность)

public:
    SliceGenerator(const Sequence<T>* src, int start_idx, int count) 
        : source(src), current_idx(start_idx), elements_left(count) {}

    Option<T> TryGetNext() override {
        if (elements_left == 0) return Option<T>();

        try {
            // Берем элемент из родителя
            Option<T> opt(source->get(current_idx++));
            
            // Если лимит задан (больше нуля), уменьшаем его
            // Если он -1, мы сюда не зайдем и лимит никогда не закончится
            if (elements_left > 0) {
                elements_left--;
            }
            return opt;
        } catch (const std::out_of_range&) {
            // Родитель кончился раньше 
            return Option<T>(); 
        }
    }

    Cardinal get_cardinality() const override {
        if (elements_left == -1) {
            // Если режем "до конца", то кардинальность зависит от остатка родителя
            // Для упрощения возвращаем Infinity, если родитель бесконечен
            return source->get_length();
        }
        return Cardinal(elements_left);
    }
};