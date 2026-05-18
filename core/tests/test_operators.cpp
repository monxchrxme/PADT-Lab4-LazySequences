#include <gtest/gtest.h>
#include <sstream>
#include "../src/sequences/mutable_array_sequence.hpp"
#include "../src/sequences/mutable_list_sequence.hpp"
#include "../src/sequences/bit_sequence.hpp"
#include "../src/sequences/sequence_io.hpp"

// operator[] test

TEST(OperatorOverloadingTest, ArraySequenceIndexing) {
    int data[] = {10, 20, 30};
    MutableArraySequence<int> seq(data, 3);

    EXPECT_EQ(seq[0], 10);
    EXPECT_EQ(seq[1], 20);
    EXPECT_EQ(seq[2], 30);

    EXPECT_THROW(seq[-1], IndexOutOfRange);
    EXPECT_THROW(seq[3], IndexOutOfRange);
}

TEST(OperatorOverloadingTest, PolymorphicIndexing) {
    int data[] = {42, 43};
    MutableListSequence<int> list_seq(data, 2);

    const Sequence<int>* base_ptr = &list_seq;

    EXPECT_EQ((*base_ptr)[0], 42);
    EXPECT_EQ((*base_ptr)[1], 43);
}

// operator<< test

TEST(OperatorOverloadingTest, OStreamOutput_Array) {
    int data[] = {1, 2, 3};
    MutableArraySequence<int> seq(data, 3);

    std::stringstream buffer;
    buffer << seq;

    EXPECT_EQ(buffer.str(), "[1, 2, 3]");
}

TEST(OperatorOverloadingTest, OStreamOutput_EmptySequence) {
    MutableListSequence<int> seq;

    std::stringstream buffer;
    buffer << seq;

    EXPECT_EQ(buffer.str(), "[]");
}

TEST(OperatorOverloadingTest, OStreamOutput_BitSequence) {
    BitSequence seq("1011");

    std::stringstream buffer;
    buffer << seq;

    EXPECT_EQ(buffer.str(), "[1, 0, 1, 1]");
}