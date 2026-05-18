#pragma once

#include "Generator.hpp"
#include "StreamExceptions.hpp"

template <typename T>
class ReadOnlyStream {
private:
    Generator<T>* generator;
    size_t position = 0;
    bool is_open = false;
    
    // Буфер на один элемент вперед
    Option<T> buffered_next;

    void fetch_next() {
        if (is_open) {
            buffered_next = generator->TryGetNext();
        }
    }

public:
    // Конструктор принимает генератор как источник данных
    // Памятью генератора управляет LazyContext
    explicit ReadOnlyStream(Generator<T>* gen) : generator(gen) {}

    void open() {
        if (is_open) return;
        is_open = true;
        position = 0;
        fetch_next(); // Предзагружаем первый элемент
    }

    void close() {
        is_open = false;
        buffered_next = Option<T>(); // Очищаем буфер
    }

    bool is_end_of_stream() const {
        if (!is_open) return true; // Закрытый поток считается исчерпанным
        return !buffered_next.is_some();
    }

    T read() {
        if (!is_open) {
            throw std::logic_error("Cannot read from a closed stream");
        }
        if (is_end_of_stream()) {
            throw EndOfStreamException();
        }

        T value = buffered_next.unwrap();
        position++;
        fetch_next();
        return value;
    }
    
    size_t get_position() const {
        return position;
    }

    bool is_can_seek() const { return false; }
    bool is_can_go_back() const { return false; }
};