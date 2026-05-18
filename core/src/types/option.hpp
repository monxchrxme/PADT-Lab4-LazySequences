#pragma once
#include "exceptions.hpp"

template <class T>
class Option {
private:
    bool _has_value;
    T _value;

public:
    // constructor for the "None" state (no value)
    Option() : _has_value(false), _value() {}

    // constructor for the "Some" state (value present)
    explicit Option(const T& value) : _has_value(true), _value(value) {}

    // check state
    [[nodiscard]] bool is_some() const { return _has_value; }
    [[nodiscard]] bool is_none() const { return !_has_value; }

    // safely extract value (raises an error if None)
    const T& unwrap() const {
        if (!_has_value) {
            throw std::logic_error("Attempted to unwrap an Option that is None");
        }
        return _value;
    }

    // extraction with a default value (does not throw errors)
    T unwrap_or(const T& default_value) const {
        return _has_value ? _value : default_value;
    }
};