#pragma once

#include "StreamInterfaces.hpp"

// Смешанный поток берет ЛЮБОЙ поток для чтения и ЛЮБОЙ поток для записи и объединяет их
template <typename TRead, typename TWrite = TRead>
class ReadWriteStream {
private:
    IReadOnlyStream<TRead>* reader;   // Абстрактный читатель
    IWriteOnlyStream<TWrite>* writer; // Абстрактный писатель

public:
    // Конструктор принимает источники (абстрактные интерфейсы) для чтения и записи
    ReadWriteStream(IReadOnlyStream<TRead>* r, IWriteOnlyStream<TWrite>* w)
        : reader(r), writer(w) {}

    void open() {
        reader->open();
        writer->open();
    }

    void close() {
        reader->close();
        writer->close();
    }

    TRead read() {
        return reader->read();
    }

    bool is_end_of_read_stream() const {
        return reader->is_end_of_stream();
    }

    size_t write(const TWrite& item) {
        return writer->write(item);
    }
};