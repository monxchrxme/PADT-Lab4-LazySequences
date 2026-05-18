#pragma once
#include "list_sequence.hpp"

template <class T>
class MutableListSequence : public ListSequence<T> {
protected:
    ListSequence<T>* get_instance() override {
        return this; // mutable struct returns itself
    }

public:
    using ListSequence<T>::ListSequence;

    ListSequence<T>* create_empty() const override {
        return new MutableListSequence<T>();
    }

    ListSequence<T>* clone() const override {
        return new MutableListSequence<T>(*this);
    }

    // pattern builder
    class Builder : public ISequenceBuilder<T> {
    private:
        MutableListSequence<T>* seq;
    public:
        Builder() {
            seq = new MutableListSequence<T>();
        }

        void append(const T& item) override {
            seq->append(item);
        }

        Sequence<T>* build() override {
            return seq;
        }
    };

    ISequenceBuilder<T>* create_builder() const override {
        return new Builder();
    }
};