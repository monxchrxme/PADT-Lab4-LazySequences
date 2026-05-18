#pragma once

#include "mutable_array_sequence.hpp"
#include <stdexcept>

template <typename T>
class SequenceWriteStream : public IWriteOnlyStream<T> {
private:
    MutableArraySequence<T>* destination;
    size_t position = 0;
    bool is_open = false;

public:
    explicit SequenceWriteStream(MutableArraySequence<T>* dest) : destination(dest) {}

    void open() {
        is_open = true;
        position = destination->get_length();
    }

    void close() {
        is_open = false;
    }

    size_t write(const T& item) {
        if (!is_open) {
            throw std::logic_error("Cannot write to a closed stream");
        }
        
        destination->append(item);
        position++;
        return position;
    }

    size_t get_position() const {
        return position;
    }
};