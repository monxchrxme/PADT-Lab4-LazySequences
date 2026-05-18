#pragma once
#include "StreamInterfaces.hpp"
#include "StreamExceptions.hpp"
#include "sequence.hpp"

template <typename T>
class SequenceReadStream : public IReadOnlyStream<T> {
private:
    const Sequence<T>* sequence;
    size_t position = 0;
    bool is_open_flag = false;

public:
    explicit SequenceReadStream(const Sequence<T>* seq) : sequence(seq) {}

    void open() override { is_open_flag = true; position = 0; }
    void close() override { is_open_flag = false; }

    bool is_end_of_stream() const override {
        if (!is_open_flag) return true;
        
        // Пытаемся безопасно узнать, не вышли ли мы за пределы кэша/массива
        try {
            int len = sequence->get_length();
            return position >= static_cast<size_t>(len);
        } catch (const std::logic_error&) {
            // Если get_length кинул ошибку (последовательность бесконечна), значит конца нет
            return false; 
        }
    }

    T read() override {
        if (!is_open_flag) throw std::logic_error("Stream closed");
        if (is_end_of_stream()) throw EndOfStreamException();

        // Если элемент еще не вычислен (в LazySequence), он вычислится сейчас
        return sequence->get(position++);
    }

    size_t get_position() const override { return position; }

    // В этом типе потоков возможно перемещение и возврат назад
    bool is_can_seek() const override { return true; }
    bool is_can_go_back() const override { return true; }
    
    size_t seek(size_t index) override {
        position = index;
        return position;
    }
};