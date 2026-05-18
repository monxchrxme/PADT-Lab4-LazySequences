#pragma once

template <class T>
class ICollection {
public:
    // destructor
    virtual ~ICollection() = default;

    // getters
    virtual const T& get(int index) const = 0;
    virtual int get_length() const = 0;

    // any collection must be able to create an empty copy of its type
    virtual ICollection<T>* create_empty() const = 0;

    // any collection must be able to make deep copy itself
    virtual ICollection<T>* clone() const = 0;
};