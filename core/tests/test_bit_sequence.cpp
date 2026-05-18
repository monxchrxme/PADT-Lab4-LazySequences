#include <gtest/gtest.h>
#include "../src/sequences/bit_sequence.hpp"
#include <string>

// anonymous namespace for test helper functions
namespace {
    // converts any sequence of bits back into a string for convenient testing
    std::string seq_to_string(const Sequence<Bit>* seq) {
        if (!seq || seq->get_length() == 0) return "";
        std::string result = "";
        for (int i = 0; i < seq->get_length(); ++i) {
            result += (seq->get(i) ? "1" : "0");
        }
        return result;
    }
}

// cunstructors test

TEST(BitSequenceTest, ConstructorValidStrings) {
    BitSequence seq1("101");
    EXPECT_EQ(seq_to_string(&seq1), "101");
    EXPECT_EQ(seq1.get_length(), 3);

    BitSequence seq2(""); // empty string
    EXPECT_EQ(seq_to_string(&seq2), "");
    EXPECT_EQ(seq2.get_length(), 0);
}

TEST(BitSequenceTest, ConstructorInvalidStringsThrow) {
    EXPECT_THROW(BitSequence("102"), std::invalid_argument);
    EXPECT_THROW(BitSequence("abc"), std::invalid_argument);
    EXPECT_THROW(BitSequence("10 1"), std::invalid_argument);
}

// basic bitwise operations (equal length)

TEST(BitSequenceTest, BitwiseNOT) {
    BitSequence seq("101001");
    BitSequence* result = seq.bitwise_not();

    EXPECT_EQ(seq_to_string(result), "010110");
    delete result;
}

TEST(BitSequenceTest, BitwiseAND_SameLength) {
    BitSequence seq1("1100");
    BitSequence seq2("1010");
    BitSequence* result = seq1.bitwise_and(&seq2);

    EXPECT_EQ(seq_to_string(result), "1000");
    delete result;
}

TEST(BitSequenceTest, BitwiseOR_SameLength) {
    BitSequence seq1("1100");
    BitSequence seq2("1010");
    BitSequence* result = seq1.bitwise_or(&seq2);

    EXPECT_EQ(seq_to_string(result), "1110");
    delete result;
}

TEST(BitSequenceTest, BitwiseXOR_SameLength) {
    BitSequence seq1("1100");
    BitSequence seq2("1010");
    BitSequence* result = seq1.bitwise_xor(&seq2);

    EXPECT_EQ(seq_to_string(result), "0110");
    delete result;
}

// alignment and variable length (right edge / LSB)

TEST(BitSequenceTest, BitwiseOperations_LeftPadding) {
    BitSequence long_seq("11111"); // len 5, value 31
    BitSequence short_seq("10");   // len 2, value 2

    BitSequence* and_res = long_seq.bitwise_and(&short_seq);
    EXPECT_EQ(seq_to_string(and_res), "00010"); // 11111 & 00010 = 00010

    BitSequence* or_res = long_seq.bitwise_or(&short_seq);
    EXPECT_EQ(seq_to_string(or_res), "11111");  // 11111 | 00010 = 11111

    BitSequence* xor_res = long_seq.bitwise_xor(&short_seq);
    EXPECT_EQ(seq_to_string(xor_res), "11101"); // 11111 ^ 00010 = 11101

    delete and_res;
    delete or_res;
    delete xor_res;
}

TEST(BitSequenceTest, BitwiseOperations_ShortFirst) {
    // verify that the argument order does not break alignment.
    BitSequence short_seq("10");
    BitSequence long_seq("11111");

    BitSequence* and_res = short_seq.bitwise_and(&long_seq);
    EXPECT_EQ(seq_to_string(and_res), "00010");

    delete and_res;
}

// edge cases

TEST(BitSequenceTest, EdgeCases_EmptySequences) {
    BitSequence empty("");
    BitSequence seq("101");

    BitSequence* not_res = empty.bitwise_not();
    EXPECT_EQ(seq_to_string(not_res), "");

    BitSequence* and_res = empty.bitwise_and(&seq);
    EXPECT_EQ(seq_to_string(and_res), "000"); // empty mask works as "000"

    BitSequence* or_res = seq.bitwise_or(&empty);
    EXPECT_EQ(seq_to_string(or_res), "101");  // 101 | 000 = 101

    delete not_res;
    delete and_res;
    delete or_res;
}

TEST(BitSequenceTest, EdgeCases_AllZerosAndOnes) {
    BitSequence zeros("0000");
    BitSequence ones("1111");
    BitSequence seq("1010");

    BitSequence* and_res = seq.bitwise_and(&zeros);
    EXPECT_EQ(seq_to_string(and_res), "0000");

    BitSequence* or_res = seq.bitwise_or(&ones);
    EXPECT_EQ(seq_to_string(or_res), "1111");

    BitSequence* xor_res = seq.bitwise_xor(&ones);
    EXPECT_EQ(seq_to_string(xor_res), "0101");

    delete and_res;
    delete or_res;
    delete xor_res;
}

// the immutability of operations

TEST(BitSequenceTest, OperationsDoNotModifyOriginals) {
    BitSequence seq1("1100");
    BitSequence seq2("1010");

    BitSequence* result = seq1.bitwise_and(&seq2);

    // verify that the original objects remain old
    EXPECT_EQ(seq_to_string(&seq1), "1100");
    EXPECT_EQ(seq_to_string(&seq2), "1010");

    delete result;
}