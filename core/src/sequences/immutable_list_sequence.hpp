#pragma once
#include "list_sequence.hpp"
#include "mutable_list_sequence.hpp"

template <class T>
class ImmutableListSequence : public ListSequence<T> {
protected:
    ListSequence<T>* get_instance() override {
        return this->clone(); // create complete copy before changing to return it
    }

public:
    using ListSequence<T>::ListSequence;

    // pattern builder
    class Builder : public ISequenceBuilder<T> {
    private:
        MutableListSequence<T>* temp_buffer;
    public:
        Builder() {
            temp_buffer = new MutableListSequence<T>();
        }
        void append(const T& item) override {
            temp_buffer->append(item);
        }

        Sequence<T>* build() override {
            int len = temp_buffer->get_length();
            T* arr = new T[len];
            int i = 0;
            for(const auto& item : *temp_buffer) {
                arr[i++] = item;
            }

            Sequence<T>* result = new ImmutableListSequence<T>(arr, len);

            delete[] arr;
            delete temp_buffer;
            return result;
        }
    };

    ISequenceBuilder<T>* create_builder() const override {
        return new Builder();
    }

    ListSequence<T>* create_empty() const override {
        return new ImmutableListSequence<T>();
    }

    ListSequence<T>* clone() const override {
        return new ImmutableListSequence<T>(*this);
    }
};