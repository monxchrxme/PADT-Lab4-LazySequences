#pragma once
#include <stdexcept>
#include <string>

class IndexOutOfRange : public std::out_of_range {
public:
    explicit IndexOutOfRange(const std::string& message)
            : std::out_of_range(message) {}
};