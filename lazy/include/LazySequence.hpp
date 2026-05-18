#pragma once

#include "sequence.hpp"               
#include "mutable_array_sequence.hpp" 
#include "LazyContext.hpp"
#include "Generator.hpp"
#include "FilterGenerator.hpp"
#include "ConcatGenerator.hpp"
#include "SliceGenerator.hpp"
#include "SingleItemGenerator.hpp"
#include "LazySequenceBuilder.hpp"
#include <stdexcept>

template <typename T>
class LazyEnumerator : public IEnumerator<T>, public IAllocatedObject {
private:
    const Sequence<T>* seq;
    int current_index = -1;

public:
    LazyEnumerator(const Sequence<T>* s) : seq(s) {}

    bool move_next() override {
        current_index++;
        try {
            seq->get(current_index);
            return true;
        } catch (const std::out_of_range&) {
            return false;
        }
    }

    const T& get_current() const override {
        return seq->get(current_index);
    }

    void reset() override {
        current_index = -1;
    }
};

template <typename T>
class LazySequence : public Sequence<T>, public IAllocatedObject {
private:
    Generator<T>* generator;
    LazyContext* context;
    
    // Кэш вычисленных значений, mutable позволяет изменять его в const-методах.
    mutable MutableArraySequence<T> cache;

    // Флаг, указывающий, что генератор исчерпан (для конечных ленивых списков)
    mutable bool is_exhausted = false;

    // вычисляет элементы вплоть до нужного индекса
    void materialize_up_to(int target_index) const {
        while (!is_exhausted && cache.get_length() <= target_index) {
            Option<T> next_val = generator->TryGetNext();
            
            if (next_val.is_some()) {  
                cache.append(next_val.unwrap()); 
            } else {
                is_exhausted = true;
            }
        }
    }

    // Вычислить список до конца (нужно быть внимательным с бесконечными списками)
    void materialize_all() const {
        while (!is_exhausted) {
            Option<T> next_val = generator->TryGetNext();
             if (next_val.is_some()) {
                cache.append(next_val.unwrap());
            } else {
                is_exhausted = true;
            }
        }
    }

public:
    // Конструктор принимает генератор и контекст, который им владеет
    LazySequence(Generator<T>* gen, LazyContext* ctx) 
        : generator(gen), context(ctx) {}

    // Базовые операции Sequence

    const T& get(int index) const override {
        if (index < 0) {
            throw std::out_of_range("Index cannot be negative");
        }
        
        materialize_up_to(index);
        
        if (index >= cache.get_length()) {
            throw std::out_of_range("Index out of range (sequence ended)");
        }
        return cache.get(index);
    }

    const T& get_first() const override {
        return get(0);
    }

    const T& get_last() const override {
        materialize_all();
        if (cache.get_length() == 0) {
            throw std::out_of_range("Sequence is empty");
        }
        return cache.get(cache.get_length() - 1);
    }

    Cardinal get_cardinality() const {
        return generator->get_cardinality();
    }

    int get_length() const override {
        Cardinal card = get_cardinality();
        if (card.is_infinite()) {
            throw std::logic_error("Cannot get integer length of an infinite sequence. Use get_cardinality().");
        }
        
        materialize_all();
        return cache.get_length();
    }

    // Сколько элементов уже вычислено
    int get_materialized_count() const {
        return cache.get_length();
    }

    // Операции изменения (возвращают новые ленивые списки)
       
    Sequence<T>* append(const T& item) override {
        auto* single_gen = context->Allocate<SingleItemGenerator<T>>(item);
        auto* single_seq = context->Allocate<LazySequence<T>>(single_gen, context);
        return this->concat(single_seq); // Текущий + Один элемент
    }

    Sequence<T>* prepend(const T& item) override {
        auto* single_gen = context->Allocate<SingleItemGenerator<T>>(item);
        auto* single_seq = context->Allocate<LazySequence<T>>(single_gen, context);
        return single_seq->concat(const_cast<LazySequence<T>*>(this)); // Один элемент + Текущий
    }

    Sequence<T>* insert_at(const T& item, int index) override {
        // Берем кусок ДО индекса, приклеиваем ЭЛЕМЕНТ, приклеиваем кусок ПОСЛЕ
        // Кусок ДО индекса (длина = index)
        auto* left_part = slice(0, index); 

        // Вставляемый элемент
        auto* single_gen = context->Allocate<SingleItemGenerator<T>>(item);
        auto* single_seq = context->Allocate<LazySequence<T>>(single_gen, context);

        // Кусок ПОСЛЕ индекса (начиная с index, и до самого конца (-1))
        auto* right_part = slice(index, -1);

        return left_part->concat(single_seq)->concat(right_part);
    }

    Sequence<T>* insert_after_materialized(const T& item) {
        // Узнаем, сколько элементов сейчас реально вычислено
        int k = this->get_materialized_count();
        
        // Вставляем элемент ровно на эту границу
        return this->insert_at(item, k);
    }

    Sequence<T>* insert_and_restart(const T& item) {
        int k = this->get_materialized_count();
        
        // Берем только то, что уже было вычислено
        auto* materialized_part = this->slice(0, k);
        
        // Вставляемый элемент
        auto* single_gen = context->Allocate<SingleItemGenerator<T>>(item);
        auto* single_seq = context->Allocate<LazySequence<T>>(single_gen, context);
        
        // Склеиваем: вычисленное + элемент + весь текущий список
        return materialized_part->concat(single_seq)->concat(const_cast<LazySequence<T>*>(this));
    }

    Sequence<T>* remove_at(int index) override {
        // Склеиваем кусок ДО индекса и кусок ПОСЛЕ индекса
        // Кусок ДО удаляемого элемента
        auto* left_part = slice(0, index);
        
        // Кусок ПОСЛЕ удаляемого элемента (пропускаем сам index)
        auto* right_part = slice(index + 1, -1); 
        
        return left_part->concat(right_part);
    }

    Sequence<T>* get_subsequence(int start_index, int end_index) const override {
        int count = end_index - start_index + 1;
        return slice(start_index, count); // Переиспользуем slice
    }


    Sequence<T>* concat(Sequence<T>* list) const override {
        auto* gen = context->Allocate<ConcatGenerator<T>>(this, list);
        return context->Allocate<LazySequence<T>>(gen, context);
    }

    Sequence<T>* slice(int index, int count, const Sequence<T>* = nullptr) const override {
        auto* gen = context->Allocate<SliceGenerator<T>>(this, index, count);
        return context->Allocate<LazySequence<T>>(gen, context);
    }

    Sequence<T>* where(bool (*predicate)(const T&)) const override {
        auto* filtered_gen = context->Allocate<FilterGenerator<T>>(this, predicate);
        return context->Allocate<LazySequence<T>>(filtered_gen, context);
    }

    T reduce(T (*reducer)(const T&, const T&), const T &initial_value) const override {
        materialize_all(); 
        
        T result = initial_value;
        for (int i = 0; i < cache.get_length(); ++i) {
            result = reducer(result, cache.get(i));
        }
        return result;
    }

    Sequence<T>* create_empty() const override {
        return slice(0, 0); // вырезаем 0 элементов
    }

    Sequence<T>* clone() const override {
        return slice(0, -1); // Вырезаем всё от 0 до бесконечности
    }

    ISequenceBuilder<T>* create_builder() const override {
        return context->Allocate<LazySequenceBuilder<T>>(context);
    }

    IEnumerator<T>* get_enumerator() const override {
        return context->Allocate<LazyEnumerator<T>>(this);
    }
};

template <typename T>
Sequence<T>* LazySequenceBuilder<T>::build() {
    // Создаем генератор на основе накопленных данных
    auto* gen = context->Allocate<CollectionGenerator<T>>(buffer);
    // Создаем ленивую последовательность
    return context->Allocate<LazySequence<T>>(gen, context);
}