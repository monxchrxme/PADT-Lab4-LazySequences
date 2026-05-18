#pragma once
#include <stdexcept>

// Исключение, выбрасываемое при попытке чтения за пределами потока
class EndOfStreamException : public std::runtime_error {
public:
    EndOfStreamException() 
        : std::runtime_error("Attempted to read past the end of the stream") {}
};
