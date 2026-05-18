#include <gtest/gtest.h>
#include "../src/sequences/list_sequence.hpp"
#include "../src/types/exceptions.hpp"
#include "../src/sequences/mutable_list_sequence.hpp"
#include "../src/sequences/algorithms.hpp"

namespace {
    int list_square_mapper(const int& x) { return x * x; }
    bool list_is_even_predicate(const int& x) { return x % 2 == 0; }
    int list_sum_reducer(const int& acc, const int& x) { return acc + x; }
}

// constructors test and testing The Rule of Five

TEST(ListSequenceTest, Constructors) {
    MutableListSequence<int> seq1;
    EXPECT_EQ(seq1.get_length(), 0);

    MutableListSequence<int> seq2(5);
    EXPECT_EQ(seq2.get_length(), 5);
    EXPECT_EQ(seq2.get(4), 0);

    int data[] = {10, 20, 30};
    MutableListSequence<int> seq3(data, 3);
    EXPECT_EQ(seq3.get_length(), 3);
    EXPECT_EQ(seq3.get_first(), 10);
}

TEST(ListSequenceTest, CopyAndMoveSemantics) {
    int data[] = {1, 2, 3};
    MutableListSequence<int> original(data, 3);

    MutableListSequence<int> copy(original);
    EXPECT_EQ(copy.get_length(), 3);
    copy.append(4);
    EXPECT_EQ(original.get_length(), 3);

    MutableListSequence<int> moved(static_cast<MutableListSequence<int>&&>(original));
    EXPECT_EQ(moved.get_length(), 3);
    EXPECT_EQ(moved.get(0), 1);
}

// getters test

TEST(ListSequenceTest, ElementAccess) {
    int data[] = {10, 20, 30};
    MutableListSequence<int> seq(data, 3);

    EXPECT_EQ(seq.get_first(), 10);
    EXPECT_EQ(seq.get_last(), 30);
    EXPECT_EQ(seq.get(1), 20);

    EXPECT_THROW(seq.get(-1), IndexOutOfRange);
    EXPECT_THROW(seq.get(3), IndexOutOfRange);
}

TEST(ListSequenceTest, GetSubsequence) {
    int data[] = {10, 20, 30, 40, 50};
    MutableListSequence<int> seq(data, 5);

    Sequence<int>* sub = seq.get_subsequence(1, 3);

    EXPECT_EQ(sub->get_length(), 3);
    EXPECT_EQ(sub->get_first(), 20);
    EXPECT_EQ(sub->get_last(), 40);

    delete sub;

    EXPECT_THROW(seq.get_subsequence(3, 1), IndexOutOfRange);
}

// modifying operations test

TEST(ListSequenceTest, Modifiers) {
    MutableListSequence<int> seq;

    seq.append(20)->prepend(10)->append(30);

    EXPECT_EQ(seq.get_length(), 3);
    EXPECT_EQ(seq.get(0), 10);
    EXPECT_EQ(seq.get(2), 30);

    seq.insert_at(25, 2);
    EXPECT_EQ(seq.get(2), 25);
    EXPECT_EQ(seq.get_length(), 4);

    seq.remove_at(1);
    EXPECT_EQ(seq.get(1), 25);
    EXPECT_EQ(seq.get_length(), 3);
}

TEST(ListSequenceTest, Concat) {
    int d1[] = {1, 2};
    int d2[] = {3, 4};
    MutableListSequence<int> seq1(d1, 2);
    MutableListSequence<int> seq2(d2, 2);

    Sequence<int>* result = seq1.concat(&seq2);
    EXPECT_EQ(result->get_length(), 4);
    EXPECT_EQ(result->get(2), 3);

    delete result;
}

// Map / Where / Reduce tests

TEST(ListSequenceTest, FunctionalEngine) {
    int data[] = {1, 2, 3, 4, 5};
    MutableListSequence<int> seq(data, 5);

//    Sequence<int>* mapped = seq.map(list_square_mapper);           // X
    Sequence<int>* mapped = Map<int, int>(&seq, list_square_mapper); // V
    EXPECT_EQ(mapped->get(4), 25);
    EXPECT_EQ(mapped->get_length(), 5);

    Sequence<int>* filtered = seq.where(list_is_even_predicate);
    EXPECT_EQ(filtered->get_length(), 2);
    EXPECT_EQ(filtered->get(0), 2);
    EXPECT_EQ(filtered->get(1), 4);

    int sum = seq.reduce(list_sum_reducer, 0);
    EXPECT_EQ(sum, 15);

    delete mapped;
    delete filtered;
}

// сross-copy and polymorphism tests (ICollection)

#include "../src/sequences/mutable_array_sequence.hpp" // needed to create a donor list

TEST(ListSequenceTest, CrossConstructorFromArray) {
    int data[] = {10, 20, 30};
    MutableArraySequence<int> arr_seq(data, 3);

    MutableListSequence<int> list_seq(arr_seq);

    EXPECT_EQ(list_seq.get_length(), 3);
    EXPECT_EQ(list_seq.get_first(), 10);
    EXPECT_EQ(list_seq.get_last(), 30);

    // check deep copying (memory isolation)
    list_seq.append(40);
    EXPECT_EQ(list_seq.get_length(), 4);
    EXPECT_EQ(arr_seq.get_length(), 3); // the original list should not change
}

TEST(ListSequenceTest, PolymorphicFactoryMethods) {
    int data[] = {100};
    MutableListSequence<int> original(data, 1);

    const ICollection<int>* base_ptr = &original;

    // polymorphic clone()
    ICollection<int>* cloned_ptr = base_ptr->clone();
    EXPECT_EQ(cloned_ptr->get_length(), 1);
    EXPECT_EQ(cloned_ptr->get(0), 100);

    // polymorphic create_empty() call
    ICollection<int>* empty_ptr = base_ptr->create_empty();
    EXPECT_EQ(empty_ptr->get_length(), 0);

    delete cloned_ptr;
    delete empty_ptr;
}