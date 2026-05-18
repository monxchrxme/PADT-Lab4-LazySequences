#include <gtest/gtest.h>
#include "../src/sequences/array_sequence.hpp"
#include "../src/types/exceptions.hpp"
#include "../src/sequences/mutable_array_sequence.hpp"
#include "../src/sequences/algorithms.hpp"

// anonymous namespace: functions are visible only inside this file (saves from linking conflicts)
namespace {
    int seq_square_mapper(const int& x) { return x * x; }
    bool seq_is_even_predicate(const int& x) { return x % 2 == 0; }
    int seq_sum_reducer(const int& acc, const int& x) { return acc + x; }
}

// constructors test and testing The Rule of Five

TEST(ArraySequenceTest, Constructors) {
    // default constructor
    MutableArraySequence<int> seq1;
    EXPECT_EQ(seq1.get_length(), 0);

    // constructor with size
    MutableArraySequence<int> seq2(5);
    EXPECT_EQ(seq2.get_length(), 5);
    EXPECT_EQ(seq2.get(4), 0); // value-initialization test

    // constructor from a raw array
    int data[] = {10, 20, 30};
    MutableArraySequence<int> seq3(data, 3);
    EXPECT_EQ(seq3.get_length(), 3);
    EXPECT_EQ(seq3.get_first(), 10);
}

TEST(ArraySequenceTest, CopyAndMoveSemantics) {
    int data[] = {1, 2, 3};
    MutableArraySequence<int> original(data, 3);

    // deep copy
    MutableArraySequence<int> copy(original);
    EXPECT_EQ(copy.get_length(), 3);
    copy.append(4);
    EXPECT_EQ(original.get_length(), 3); // changing the copy does not change the original

    // move
    MutableArraySequence<int> moved(static_cast<MutableArraySequence<int>&&>(original));
    EXPECT_EQ(moved.get_length(), 3);
    EXPECT_EQ(moved.get(0), 1);
}

// getters test

TEST(ArraySequenceTest, ElementAccess) {
    int data[] = {10, 20, 30};
    MutableArraySequence<int> seq(data, 3);

    EXPECT_EQ(seq.get_first(), 10);
    EXPECT_EQ(seq.get_last(), 30);
    EXPECT_EQ(seq.get(1), 20);

    EXPECT_THROW(seq.get(-1), IndexOutOfRange);
    EXPECT_THROW(seq.get(3), IndexOutOfRange);
}

TEST(ArraySequenceTest, GetSubsequence) {
    int data[] = {10, 20, 30, 40, 50};
    MutableArraySequence<int> seq(data, 5);

    Sequence<int>* sub = seq.get_subsequence(1, 3);

    EXPECT_EQ(sub->get_length(), 3);
    EXPECT_EQ(sub->get_first(), 20);
    EXPECT_EQ(sub->get_last(), 40);

    delete sub;

    EXPECT_THROW(seq.get_subsequence(3, 1), IndexOutOfRange);
}

// modifying operations test

TEST(ArraySequenceTest, Modifiers) {
    MutableArraySequence<int> seq;

    // call chains test (Fluent Interface)
    seq.append(20)->prepend(10)->append(30);
    // result: 10, 20, 30

    EXPECT_EQ(seq.get_length(), 3);
    EXPECT_EQ(seq.get(0), 10);
    EXPECT_EQ(seq.get(2), 30);

    seq.insert_at(25, 2); // 10, 20, 25, 30
    EXPECT_EQ(seq.get(2), 25);
    EXPECT_EQ(seq.get_length(), 4);

    seq.remove_at(1); // 10, 25, 30
    EXPECT_EQ(seq.get(1), 25);
    EXPECT_EQ(seq.get_length(), 3);
}

TEST(ArraySequenceTest, Concat) {
    int d1[] = {1, 2};
    int d2[] = {3, 4};
    MutableArraySequence<int> seq1(d1, 2);
    MutableArraySequence<int> seq2(d2, 2);

    Sequence<int>* result = seq1.concat(&seq2);
    EXPECT_EQ(result->get_length(), 4);
    EXPECT_EQ(result->get(2), 3);

    delete result;
}

// Map / Where / Reduce tests

TEST(ArraySequenceTest, FunctionalEngine) {
    int data[] = {1, 2, 3, 4, 5};
    MutableArraySequence<int> seq(data, 5);

    // Map: squares
    Sequence<int>* mapped = Map<int, int>(&seq, seq_square_mapper);
    EXPECT_EQ(mapped->get(4), 25);
    EXPECT_EQ(mapped->get_length(), 5);

    // Where: only even
    Sequence<int>* filtered = seq.where(seq_is_even_predicate);
    EXPECT_EQ(filtered->get_length(), 2);
    EXPECT_EQ(filtered->get(0), 2);
    EXPECT_EQ(filtered->get(1), 4);

    // Reduce: sum
    int sum = seq.reduce(seq_sum_reducer, 0);
    EXPECT_EQ(sum, 15);

    delete mapped;
    delete filtered;
}

// сross-copy and polymorphism tests (ICollection)

#include "../src/sequences/mutable_list_sequence.hpp" // needed to create a donor list


TEST(ArraySequenceTest, CrossConstructorFromList) {
    int data[] = {1, 2, 3, 4, 5};
    MutableListSequence<int> list_seq(data, 5);

    // calling explicit cross-constructor
    // the array accepts the ICollection interface (which hides the list)
    MutableArraySequence<int> arr_seq(list_seq);

    EXPECT_EQ(arr_seq.get_length(), 5);
    EXPECT_EQ(arr_seq.get(0), 1);
    EXPECT_EQ(arr_seq.get(4), 5);

    // check deep copying (memory isolation)
    arr_seq.append(99);
    EXPECT_EQ(arr_seq.get_length(), 6);
    EXPECT_EQ(list_seq.get_length(), 5); // the original list should not change
}

TEST(ArraySequenceTest, PolymorphicFactoryMethods) {
    int data[] = {42, 43};
    MutableArraySequence<int> original(data, 2);

    // upcast: we look at the array as a basic interface (exclusively with its 3 methods)
    const ICollection<int>* base_ptr = &original;

    // polymorphic clone() call. A new ArraySequence will be returned, but under the mask of ICollection
    ICollection<int>* cloned_ptr = base_ptr->clone();
    EXPECT_EQ(cloned_ptr->get_length(), 2);
    EXPECT_EQ(cloned_ptr->get(0), 42);
    EXPECT_EQ(cloned_ptr->get(1), 43);

    // polymorphic create_empty() call
    ICollection<int>* empty_ptr = base_ptr->create_empty();
    EXPECT_EQ(empty_ptr->get_length(), 0);

    delete cloned_ptr;
    delete empty_ptr;
}