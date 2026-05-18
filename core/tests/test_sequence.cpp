#include <gtest/gtest.h>
#include "../src/sequences/mutable_array_sequence.hpp"
#include "../src/sequences/immutable_list_sequence.hpp"
#include "../src/types/exceptions.hpp"
#include "../src/sequences/algorithms.hpp"
#include <string>

int square(const int& x) { return x * x; }
bool is_even_seq(const int& x) { return x % 2 == 0; }
std::string int_to_string(const int& x) { return "Number: " + std::to_string(x); }
int sum_reducer(const int& current, const int& acc) { return acc + current; }


// CppIterator test

TEST(SequenceBaseTest, RangeBasedForLoop) {
    int data[] = {10, 20, 30, 40, 50};
    MutableArraySequence<int> seq(data, 5);

    int sum = 0;
    int iterations = 0;
    for (const auto& item : seq) {
        sum += item;
        iterations++;
    }

    EXPECT_EQ(iterations, 5);
    EXPECT_EQ(sum, 150);
}

// GetSubsequence test

TEST(SequenceBaseTest, GetSubsequence_ValidRange) {
    int data[] = {1, 2, 3, 4, 5};
    ImmutableListSequence<int> seq(data, 5);

    Sequence<int>* sub = seq.get_subsequence(1, 3); // Должны получить {2, 3, 4}

    ASSERT_EQ(sub->get_length(), 3);
    EXPECT_EQ(sub->get(0), 2);
    EXPECT_EQ(sub->get(2), 4);

    EXPECT_NE(dynamic_cast<ImmutableListSequence<int>*>(sub), nullptr);

    delete sub;
}

TEST(SequenceBaseTest, GetSubsequence_ThrowsOnInvalidIndices) {
    int data[] = {1, 2, 3};
    MutableArraySequence<int> seq(data, 3);

    // Проверяем все виды выхода за границы
    EXPECT_THROW(seq.get_subsequence(-1, 2), IndexOutOfRange);
    EXPECT_THROW(seq.get_subsequence(0, 5), IndexOutOfRange);
    EXPECT_THROW(seq.get_subsequence(2, 1), IndexOutOfRange); // start > end
}


// Concat test

TEST(SequenceBaseTest, Concat_DifferentImplementations) {
    int data1[] = {1, 2};
    int data2[] = {3, 4, 5};


    ImmutableListSequence<int> seq1(data1, 2);
    MutableArraySequence<int> seq2(data2, 3);


    Sequence<int>* result = seq1.concat(&seq2);

    ASSERT_EQ(result->get_length(), 5);
    EXPECT_EQ(result->get(0), 1);
    EXPECT_EQ(result->get(2), 3);
    EXPECT_EQ(result->get(4), 5);

    EXPECT_NE(dynamic_cast<ImmutableListSequence<int>*>(result), nullptr);

    delete result;
}

TEST(SequenceBaseTest, Concat_WithNullptr) {
    int data[] = {1, 2};
    MutableArraySequence<int> seq(data, 2);

    Sequence<int>* result = seq.concat(nullptr);

    ASSERT_EQ(result->get_length(), 2);
    EXPECT_EQ(result->get(0), 1);
    EXPECT_NE(result, &seq);

    delete result;
}


// Map, Where, Reduce test

TEST(SequenceBaseTest, Map_TransformsElements_SameType) {
    int data[] = {1, 2, 3};
    MutableArraySequence<int> seq(data, 3);

    Sequence<int>* mapped = Map<int, int>(&seq, square);

    ASSERT_EQ(mapped->get_length(), 3);
    EXPECT_EQ(mapped->get(0), 1);
    EXPECT_EQ(mapped->get(1), 4);
    EXPECT_EQ(mapped->get(2), 9);

    delete mapped;
}

TEST(SequenceBaseTest, Map_ChangesType_IntToString) {
    int data[] = {10, 20};
    ImmutableListSequence<int> seq(data, 2);

    Sequence<std::string>* mapped = Map<int, std::string>(&seq, int_to_string);

    ASSERT_EQ(mapped->get_length(), 2);
    EXPECT_EQ(mapped->get(0), "Number: 10");
    EXPECT_EQ(mapped->get(1), "Number: 20");

    delete mapped;
}

TEST(SequenceBaseTest, Where_FiltersElements) {
    int data[] = {1, 2, 3, 4, 5, 6};
    ImmutableListSequence<int> seq(data, 6);

    Sequence<int>* filtered = seq.where(is_even_seq);

    ASSERT_EQ(filtered->get_length(), 3);
    EXPECT_EQ(filtered->get(0), 2);
    EXPECT_EQ(filtered->get(1), 4);
    EXPECT_EQ(filtered->get(2), 6);

    EXPECT_NE(dynamic_cast<ImmutableListSequence<int>*>(filtered), nullptr);

    delete filtered;
}

TEST(SequenceBaseTest, Reduce_AggregatesElements) {
    int data[] = {1, 2, 3, 4};
    MutableArraySequence<int> seq(data, 4);

    int result = seq.reduce(sum_reducer, 10);

    EXPECT_EQ(result, 20);
}