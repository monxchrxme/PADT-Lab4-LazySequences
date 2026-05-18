#pragma once

#include "ReadOnlyStream.hpp"
#include "WriteOnlyStream.hpp"

// Смешанный поток просто объединяет в себе функционал чтения и записи
template <typename TRead, typename TWrite = TRead>
class ReadWriteStream {
private:
    ReadOnlyStream<TRead> reader;
    WriteOnlyStream<TWrite> writer;

public:
    // Конструктор принимает источники для чтения и записи
    ReadWriteStream(Generator<TRead>* read_gen, MutableArraySequence<TWrite>* write_dest)
        : reader(read_gen), writer(write_dest) {}

    void open() {
        reader.open();
        writer.open();
    }

    void close() {
        reader.close();
        writer.close();
    }

    TRead read() {
        return reader.read();
    }

    bool is_end_of_read_stream() const {
        return reader.is_end_of_stream();
    }

    size_t write(const TWrite& item) {
        return writer.write(item);
    }
};