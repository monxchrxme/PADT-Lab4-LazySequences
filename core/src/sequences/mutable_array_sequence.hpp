#pragma once
#include "array_sequence.hpp"

template <class T>
class MutableArraySequence : public ArraySequence<T> {
protected:
    ArraySequence<T>* get_instance() override {
        return this; // mutable struct returns itself
    }

public:
    // inherit all constructors of the base class
    using ArraySequence<T>::ArraySequence;

    ArraySequence<T>* create_empty() const override {
        return new MutableArraySequence<T>();
    }

    ArraySequence<T>* clone() const override {
        return new MutableArraySequence<T>(*this);
    }

    // pattern builder
    class Builder : public ISequenceBuilder<T> {
    private:
        MutableArraySequence<T>* seq;
    public:
        Builder() {
            seq = new MutableArraySequence<T>();
        }

        void append(const T& item) override {
            seq->append(item);
        }

        Sequence<T>* build() override {
            return seq; // return the finished object, transferring ownership of the memory
        }
    };

    ISequenceBuilder<T>* create_builder() const override {
        return new Builder();
    }
};
