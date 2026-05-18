#pragma once
#include "StreamInterfaces.hpp"
#include "StreamExceptions.hpp"
#include "Generator.hpp"

template <typename T>
class GeneratorReadStream : public IReadOnlyStream<T> {
private:
    Generator<T>* generator;
    size_t position = 0;
    bool is_open_flag = false;
    Option<T> buffered_next;

    void fetch_next() {
        if (is_open_flag) {
            buffered_next = generator->TryGetNext();
        }
    }

public:
    // Конструктор принимает генератор как источник данных
    // Памятью генератора управляет LazyContext
    explicit GeneratorReadStream(Generator<T>* gen) : generator(gen) {}

    void open() override {
        if (is_open_flag) return;
        is_open_flag = true;
        position = 0;
        fetch_next();
    }

    void close() override {
        is_open_flag = false;
        buffered_next = Option<T>();
    }

    bool is_end_of_stream() const override {
        if (!is_open_flag) return true;
        return !buffered_next.is_some();
    }

    T read() override {
        if (!is_open_flag) throw std::logic_error("Stream closed");
        if (is_end_of_stream()) throw EndOfStreamException();

        T value = buffered_next.unwrap();
        position++;
        fetch_next();
        
        return value;
    }
    
    size_t get_position() const override { return position; }

    // Генератор не умеет ходить назад
    bool is_can_seek() const override { return false; }
    bool is_can_go_back() const override { return false; }
    size_t seek(size_t /*index*/) override {
        throw std::logic_error("Seeking not supported in GeneratorStream");
    }
};