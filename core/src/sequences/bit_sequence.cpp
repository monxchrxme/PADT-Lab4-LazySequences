#include "bit_sequence.hpp"

class BitEnumerator : public IEnumerator<Bit> {
private:
    const BitSequence* seq;
    int current_index;
public:
    explicit BitEnumerator(const BitSequence* sequence) : seq(sequence), current_index(-1) {}

    bool move_next() override {
        current_index++;
        return current_index < seq->get_length();
    }

    [[nodiscard]] const Bit& get_current() const override {
        return seq->get(current_index);
    }

    void reset() override {
        current_index = -1;
    }
};

BitSequence::BitSequence() : bit_count(0) {}

BitSequence::BitSequence(int size) : bit_count(0) {
    for (int i = 0; i < size; ++i) {
        this->append(Bit(false));
    }
}

BitSequence::BitSequence(const std::string& bit_string) : bit_count(0) {
    for (char c : bit_string) {
        if (c == '1') this->append(Bit(true));
        else if (c == '0') this->append(Bit(false));
        else throw std::invalid_argument("Bit string must contain only '0' or '1'");
    }
}

BitSequence::BitSequence(const BitSequence& other) {
    this->bit_count = other.bit_count;
    for (int i = 0; i < other.bytes.get_length(); ++i) {
        this->bytes.append(other.bytes.get(i));
    }
}

void BitSequence::set_bit_at(int index, bool value) {
    if (index < 0 || index >= bit_count) {
        throw std::out_of_range("Index out of range");
    }

    int byte_idx = get_byte_index(index);
    int offset = get_bit_offset(index);

    uint8_t current_byte = bytes.get(byte_idx);

    if (value) {
        current_byte |= (1 << offset);
    } else {
        current_byte &= ~(1 << offset);
    }

    bytes.remove_at(byte_idx);
    bytes.insert_at(current_byte, byte_idx);
}

int BitSequence::get_length() const {
    return bit_count;
}

const Bit& BitSequence::get(int index) const {
    if (index < 0 || index >= bit_count) {
        throw std::out_of_range("Index out of range");
    }

    uint8_t current_byte = bytes.get(get_byte_index(index));
    bool bit_value = (current_byte >> get_bit_offset(index)) & 1;

    // return reference to one or zero const object
    return bit_value ? this->bit_one : this->bit_zero;
}

Sequence<Bit>* BitSequence::clone() const {
    return new BitSequence(*this);
}

Sequence<Bit>* BitSequence::create_empty() const {
    return new BitSequence();
}

Sequence<Bit>* BitSequence::append(const Bit& item) {
    if (bit_count % 8 == 0) {
        bytes.append(0);
    }
    bit_count++;
    set_bit_at(bit_count - 1, item.get());
    return this;
}

Sequence<Bit>* BitSequence::prepend(const Bit& item) {
    return this->insert_at(item, 0);
}

Sequence<Bit>* BitSequence::insert_at(const Bit& item, int index) {
    if (index < 0 || index > bit_count) {
        throw std::out_of_range("Index out of range");
    }
    if (index == bit_count) {
        return append(item);
    }

    this->append(Bit(false));

    for (int i = bit_count - 1; i > index; --i) {
        this->set_bit_at(i, this->get(i - 1).get());
    }

    this->set_bit_at(index, item.get());
    return this;
}

Sequence<Bit>* BitSequence::remove_at(int index) {
    if (index < 0 || index >= bit_count) {
        throw std::out_of_range("Index out of range");
    }

    for (int i = index; i < bit_count - 1; ++i) {
        this->set_bit_at(i, this->get(i + 1).get());
    }

    bit_count--;

    if (bit_count % 8 == 0 && bit_count > 0) {
        bytes.remove_at(bytes.get_length() - 1);
    } else if (bit_count == 0 && bytes.get_length() > 0) {
        bytes.remove_at(0);
    }

    return this;
}

IEnumerator<Bit>* BitSequence::get_enumerator() const {
    return new BitEnumerator(this);
}

ISequenceBuilder<Bit>* BitSequence::create_builder() const {
    class BitBuilder : public ISequenceBuilder<Bit> {
        BitSequence* seq;
    public:
        BitBuilder() {
            seq = new BitSequence();
        }
        void append(const Bit& item) override {
            seq->append(item);
        }
        Sequence<Bit>* build() override {
            Sequence<Bit>* res = seq;
            seq = new BitSequence();
            return res;
        }
    };
    return new BitBuilder();
}

// left padding
BitSequence BitSequence::get_padded_to(int target_length) const {
    if (this->bit_count >= target_length) {
        return *this;
    }

    BitSequence padded_seq;
    int zeros_to_add = target_length - this->bit_count;

    for (int i = 0; i < zeros_to_add; ++i) {
        padded_seq.append(Bit(false));
    }

    for (int i = 0; i < this->bit_count; ++i) {
        padded_seq.append(this->get(i));
    }

    return padded_seq;
}

BitSequence* BitSequence::bitwise_and(const BitSequence* other) const {
    int target_len = std::max(this->bit_count, other->bit_count);

    BitSequence left = this->get_padded_to(target_len);
    BitSequence right = other->get_padded_to(target_len);

    auto* res = new BitSequence();
    for (int i = 0; i < left.bytes.get_length(); ++i) {
        res->bytes.append(left.bytes.get(i) & right.bytes.get(i));
    }
    res->bit_count = target_len;
    return res;
}

BitSequence* BitSequence::bitwise_or(const BitSequence* other) const {
    int target_len = std::max(this->bit_count, other->bit_count);

    BitSequence left = this->get_padded_to(target_len);
    BitSequence right = other->get_padded_to(target_len);

    auto* res = new BitSequence();
    for (int i = 0; i < left.bytes.get_length(); ++i) {
        res->bytes.append(left.bytes.get(i) | right.bytes.get(i));
    }
    res->bit_count = target_len;
    return res;
}

BitSequence* BitSequence::bitwise_xor(const BitSequence* other) const {
    int target_len = std::max(this->bit_count, other->bit_count);

    BitSequence left = this->get_padded_to(target_len);
    BitSequence right = other->get_padded_to(target_len);

    auto* res = new BitSequence();
    for (int i = 0; i < left.bytes.get_length(); ++i) {
        res->bytes.append(left.bytes.get(i) ^ right.bytes.get(i));
    }
    res->bit_count = target_len;
    return res;
}

BitSequence* BitSequence::bitwise_not() const {
    auto* res = new BitSequence();
    for (int i = 0; i < this->bytes.get_length(); ++i) {

        res->bytes.append(~this->bytes.get(i));
    }
    res->bit_count = this->bit_count;
    return res;
}
