#pragma once
#include "Generator.hpp"
#include "sequence.hpp"
#include <stdexcept>

template <typename T>
class ConcatGenerator : public Generator<T> {
private:
    const Sequence<T>* first_seq;
    const Sequence<T>* second_seq;
    int first_idx = 0;
    int second_idx = 0;
    bool first_exhausted = false;

public:
    ConcatGenerator(const Sequence<T>* first, const Sequence<T>* second)
        : first_seq(first), second_seq(second) {}

    Option<T> TryGetNext() override {
        // Сначала исчерпываем первый список
        if (!first_exhausted) {
            try {
                return Option<T>(first_seq->get(first_idx++));
            } catch (const std::out_of_range&) {
                first_exhausted = true; // Первый кончился, переключаемся на второй
            }
        }
        
        // Затем читаем из второго
        try {
            return Option<T>(second_seq->get(second_idx++));
        } catch (const std::out_of_range&) {
            return Option<T>(); // Кончились оба
        }
    }

    Cardinal get_cardinality() const override {
        // Если хотя бы один бесконечен, сумма тоже станет бесконечностью
        auto get_card = [](const Sequence<T>* seq) {
            try { return Cardinal(seq->get_length()); } 
            catch (const std::logic_error&) { return Cardinal::Infinity(); }
        };
        return get_card(first_seq) + get_card(second_seq);
    }
};