#include <gtest/gtest.h>
#include "../src/sequences/mutable_array_sequence.hpp"
#include "../src/sequences/immutable_array_sequence.hpp"

// Mutable structure test
// EXPECTATION: The method returns the same address (this), and the original object is modified

// fixtures for easy reusing test data
class MutableArrayTest : public ::testing::Test {
protected:
    // prepare standard data for each test
    int data[3] = {1, 2, 3};
};

TEST_F(MutableArrayTest, AppendModifiesInPlace) {
    MutableArraySequence<int> seq(data, 3);
    Sequence<int>* result = seq.append(4);

    EXPECT_EQ(result, &seq);
    EXPECT_EQ(seq.get_length(), 4);
    EXPECT_EQ(seq.get_last(), 4);
}

TEST_F(MutableArrayTest, PrependModifiesInPlace) {
    MutableArraySequence<int> seq(data, 3);
    Sequence<int>* result = seq.prepend(0);

    EXPECT_EQ(result, &seq);
    EXPECT_EQ(seq.get_length(), 4);
    EXPECT_EQ(seq.get_first(), 0);
    EXPECT_EQ(seq.get(1), 1);
}

TEST_F(MutableArrayTest, InsertAtModifiesInPlace) {
    MutableArraySequence<int> seq(data, 3);
    Sequence<int>* result = seq.insert_at(99, 1);

    EXPECT_EQ(result, &seq);
    EXPECT_EQ(seq.get_length(), 4);
    EXPECT_EQ(seq.get(1), 99);
    EXPECT_EQ(seq.get(2), 2);
}

TEST_F(MutableArrayTest, RemoveAtModifiesInPlace) {
    MutableArraySequence<int> seq(data, 3);
    Sequence<int>* result = seq.remove_at(1); // delete "2"

    EXPECT_EQ(result, &seq);
    EXPECT_EQ(seq.get_length(), 2);
    EXPECT_EQ(seq.get(1), 3);
}

// Immutable structure test
// EXPECTATION: The method returns a NEW address; the original remains UNCHANGED

// fixtures for easy reusing test data
class ImmutableArrayTest : public ::testing::Test {
protected:
    int data[3] = {1, 2, 3};
};

TEST_F(ImmutableArrayTest, AppendCreatesCopy) {
    ImmutableArraySequence<int> seq(data, 3);
    Sequence<int>* result = seq.append(4);

    EXPECT_NE(result, &seq); // Addresses MUST differ

    // original untouched
    EXPECT_EQ(seq.get_length(), 3);
    EXPECT_EQ(seq.get_last(), 3);

    // the copy contains changes
    EXPECT_EQ(result->get_length(), 4);
    EXPECT_EQ(result->get_last(), 4);

    delete result;
}

TEST_F(ImmutableArrayTest, PrependCreatesCopy) {
    ImmutableArraySequence<int> seq(data, 3);
    Sequence<int>* result = seq.prepend(0);

    EXPECT_NE(result, &seq);

    EXPECT_EQ(seq.get_length(), 3);
    EXPECT_EQ(seq.get_first(), 1);

    EXPECT_EQ(result->get_length(), 4);
    EXPECT_EQ(result->get_first(), 0);
    EXPECT_EQ(result->get(1), 1);

    delete result;
}

TEST_F(ImmutableArrayTest, InsertAtCreatesCopy) {
    ImmutableArraySequence<int> seq(data, 3);
    Sequence<int>* result = seq.insert_at(99, 1);

    EXPECT_NE(result, &seq);

    EXPECT_EQ(seq.get_length(), 3);
    EXPECT_EQ(seq.get(1), 2);

    EXPECT_EQ(result->get_length(), 4);
    EXPECT_EQ(result->get(1), 99);

    delete result;
}

TEST_F(ImmutableArrayTest, RemoveAtCreatesCopy) {
    ImmutableArraySequence<int> seq(data, 3);
    Sequence<int>* result = seq.remove_at(1); // delete "2" from copy

    EXPECT_NE(result, &seq);

    EXPECT_EQ(seq.get_length(), 3);
    EXPECT_EQ(seq.get(1), 2);

    EXPECT_EQ(result->get_length(), 2);
    EXPECT_EQ(result->get(0), 1);
    EXPECT_EQ(result->get(1), 3);

    delete result;
}

// call chains (Fluent API) for Immutable structs

TEST_F(ImmutableArrayTest, MethodChaining) {
    ImmutableArraySequence<int> seq(data, 3); // {1, 2, 3}

    // in the Immutable paradigm, every method call generates a NEW object on the heap
    // if we write "seq.append(4)->append(5)", the intermediate object will leak memory
    // therefore, call chains are constructed with careful handling of pointers
    Sequence<int>* step1 = seq.append(4);       // {1, 2, 3, 4}
    Sequence<int>* step2 = step1->prepend(0);   // {0, 1, 2, 3, 4}
    Sequence<int>* final_res = step2->remove_at(2); // delete '2' -> {0, 1, 3, 4}

    EXPECT_EQ(seq.get_length(), 3);

    EXPECT_EQ(final_res->get_length(), 4);
    EXPECT_EQ(final_res->get(0), 0);
    EXPECT_EQ(final_res->get(2), 3);

    // delete all intermediate clones
    delete step1;
    delete step2;
    delete final_res;
}