#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>
#include "../interfaces/sequence.hpp"
#include "mutable_array_sequence.hpp"
#include "../types/bit.hpp"

class BitSequence : public Sequence<Bit> {
private:
    MutableArraySequence<uint8_t> bytes;
    int bit_count;

    // to get Bit type by reference (ICollection requires)
    Bit bit_zero{false};
    Bit bit_one{true};

    static int get_byte_index(int bit_index) {
        return bit_index / 8;
    }
    static int get_bit_offset(int bit_index) {
            return bit_index % 8;
    }

    void set_bit_at(int index, bool value);
    BitSequence get_padded_to(int target_length) const;

public:
    BitSequence();
    explicit BitSequence(int size);
    explicit BitSequence(const std::string& bit_string);
    BitSequence(const BitSequence& other);

    ~BitSequence() override = default;

    int get_length() const override;
    const Bit& get(int index) const override;
    Sequence<Bit>* clone() const override;
    Sequence<Bit>* create_empty() const override;

    Sequence<Bit>* append(const Bit& item) override;
    Sequence<Bit>* prepend(const Bit& item) override;
    Sequence<Bit>* insert_at(const Bit& item, int index) override;
    Sequence<Bit>* remove_at(int index) override;

    IEnumerator<Bit>* get_enumerator() const override;
    ISequenceBuilder<Bit>* create_builder() const override;

    BitSequence* bitwise_and(const BitSequence* other) const;
    BitSequence* bitwise_or(const BitSequence* other) const;
    BitSequence* bitwise_xor(const BitSequence* other) const;
    BitSequence* bitwise_not() const;
};