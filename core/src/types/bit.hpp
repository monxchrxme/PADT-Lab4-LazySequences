#pragma once
#include <iostream>

class Bit {
public:
    // implicit constructor from bool and int (allows writing Bit b = 1;)
    Bit(bool v = false) : value(v) {}
    Bit(int v) : value(v != 0) {}

    bool get() const {
        return value;
    }

    // casting back to bool
    operator bool() const {
        return value;
    }

    // bitwise operations

    // AND
    Bit operator&(const Bit& other) const {
        return Bit(value && other.value);
    }

    // OR
    Bit operator|(const Bit& other) const {
        return Bit(value || other.value);
    }

    // XOR
    Bit operator^(const Bit& other) const {
        return Bit(value != other.value);
    }

    // NOT
    Bit operator~() const {
        return Bit(!value);
    }

    // comparison operators
    bool operator==(const Bit& other) const {
        return value == other.value;
    }

    bool operator!=(const Bit& other) const {
        return value != other.value;
    }

    friend std::ostream& operator<<(std::ostream& os, const Bit& bit) {
        os << (bit.value ? "1" : "0");
        return os;
    }

private:
    bool value;
};