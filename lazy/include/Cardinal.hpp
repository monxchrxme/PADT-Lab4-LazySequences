#pragma once

#include <stdexcept>
#include <string>

class Cardinal {
private:
    int value;
    bool infinite;

    Cardinal(int v, bool inf) : value(v), infinite(inf) {}

public:
    // По умолчанию создаем как 0
    Cardinal() : value(0), infinite(false) {}

    // Неявное преобразование из обычного числа
    Cardinal(int v) : value(v), infinite(false) {
        if (v < 0) {
            throw std::invalid_argument("Cardinal cannot be negative");
        }
    }

    static Cardinal Infinity() {
        return Cardinal(0, true);
    }

    bool is_infinite() const {
        return infinite;
    }

    // Безопасное получение числа (бросает исключение, если это бесконечность)
    int get_value() const {
        if (infinite) {
            throw std::logic_error("Cannot get integer value of Infinity");
        }
        return value;
    }

    // Перегрузка операторов для удобства 

    Cardinal operator+(const Cardinal& other) const {
        if (this->infinite || other.infinite) {
            return Cardinal::Infinity();
        }
        return Cardinal(this->value + other.value);
    }

    bool operator==(const Cardinal& other) const {
        if (this->infinite && other.infinite) return true;
        if (!this->infinite && !other.infinite) return this->value == other.value;
        return false;
    }

    bool operator!=(const Cardinal& other) const {
        return !(*this == other);
    }

    // Строковое представление для отладки и UI
    std::string to_string() const {
        return infinite ? "Infinity" : std::to_string(value);
    }
};