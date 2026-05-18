#pragma once
#include <cstddef> // для size_t

template <typename T>
class IReadOnlyStream {
public:
    virtual ~IReadOnlyStream() = default;

    virtual void open() = 0;
    virtual void close() = 0;
    
    virtual T read() = 0;
    virtual bool is_end_of_stream() const = 0;
    
    virtual size_t get_position() const = 0;
    
    virtual bool is_can_seek() const = 0;
    virtual size_t seek(size_t index) = 0;
    virtual bool is_can_go_back() const = 0;
};

template <typename T>
class IWriteOnlyStream {
public:
    virtual ~IWriteOnlyStream() = default;

    virtual void open() = 0;
    virtual void close() = 0;
    
    virtual size_t write(const T& item) = 0;
    virtual size_t get_position() const = 0;
};