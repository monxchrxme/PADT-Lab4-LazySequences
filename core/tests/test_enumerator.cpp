#include <gtest/gtest.h>
#include "../src/sequences/mutable_array_sequence.hpp"
#include "../src/sequences/mutable_list_sequence.hpp"
#include "../src/types/exceptions.hpp"

// iterating over basic collections (Happy Path)

TEST(EnumeratorTest, ArraySequenceFullTraversal) {
    int data[] = {10, 20, 30};
    MutableArraySequence<int> seq(data, 3);
    IEnumerator<int>* it = seq.get_enumerator();

    EXPECT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 10);
    EXPECT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 20);
    EXPECT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 30);
    EXPECT_FALSE(it->move_next());

    delete it;
}

TEST(EnumeratorTest, ListSequenceFullTraversal) {
    int data[] = {100, 200};
    MutableListSequence<int> seq(data, 2);
    IEnumerator<int>* it = seq.get_enumerator();

    int sum = 0;
    while (it->move_next()) {
        sum += it->get_current();
    }

    EXPECT_EQ(sum, 300);
    EXPECT_FALSE(it->move_next());

    delete it;
}

// edge cases: empty collections

TEST(EnumeratorTest, EmptyArraySequence) {
    MutableArraySequence<int> seq;
    IEnumerator<int>* it = seq.get_enumerator();

    EXPECT_FALSE(it->move_next());

    EXPECT_THROW({
        volatile auto val = it->get_current();
        }, IndexOutOfRange);

    delete it;
}

TEST(EnumeratorTest, EmptyListSequence) {
    MutableListSequence<int> seq;
    IEnumerator<int>* it = seq.get_enumerator();

    EXPECT_FALSE(it->move_next());
    EXPECT_THROW({
        volatile auto val = it->get_current();
        }, IndexOutOfRange);

    delete it;
}

// security check: exceptions before start and after finish

TEST(EnumeratorTest, OutOfBoundsAccessThrows) {
    int data[] = {42};
    MutableArraySequence<int> seq(data, 1);
    IEnumerator<int>* it = seq.get_enumerator();

    EXPECT_THROW({
        volatile auto val = it->get_current();
        }, IndexOutOfRange);

    EXPECT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 42);

    EXPECT_FALSE(it->move_next());

    EXPECT_THROW({
        volatile auto val = it->get_current();
        }, IndexOutOfRange);

    delete it;
}

// reset method test

TEST(EnumeratorTest, ResetWorksProperly) {
    int data[] = {1, 2, 3};
    MutableListSequence<int> seq(data, 3);
    IEnumerator<int>* it = seq.get_enumerator();

    it->move_next();
    it->move_next();
    EXPECT_EQ(it->get_current(), 2);

    it->reset();
    EXPECT_THROW(it->get_current(), IndexOutOfRange);

    EXPECT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 1);

    while(it->move_next());
    EXPECT_FALSE(it->move_next());

    it->reset();
    EXPECT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 1);

    delete it;
}

// isolation: independent iterators

TEST(EnumeratorTest, IndependentIterators) {
    int data[] = {10, 20, 30};
    MutableArraySequence<int> seq(data, 3);

    IEnumerator<int>* it1 = seq.get_enumerator();
    IEnumerator<int>* it2 = seq.get_enumerator();

    EXPECT_TRUE(it1->move_next());
    EXPECT_EQ(it1->get_current(), 10);

    EXPECT_THROW(it2->get_current(), IndexOutOfRange);
    EXPECT_TRUE(it2->move_next());
    EXPECT_EQ(it2->get_current(), 10);

    EXPECT_TRUE(it1->move_next());
    EXPECT_TRUE(it1->move_next());
    EXPECT_EQ(it1->get_current(), 30);

    EXPECT_EQ(it2->get_current(), 10);

    delete it1;
    delete it2;
}

// //  Cpp-style Range-based for loop (begin() / end())

TEST(EnumeratorTest, CppRangeBasedForLoop_Array) {
    int data[] = {1, 2, 3, 4, 5};
    MutableArraySequence<int> seq(data, 5);

    int sum = 0;
    int count = 0;

    for (const auto& item : seq) {
        sum += item;
        count++;
    }

    EXPECT_EQ(sum, 15);
    EXPECT_EQ(count, 5);
}

TEST(EnumeratorTest, CppRangeBasedForLoop_List) {
    int data[] = {10, 20, 30};
    MutableListSequence<int> seq(data, 3);

    int expected[] = {10, 20, 30};
    int i = 0;

    for (const auto& item : seq) {
        EXPECT_EQ(item, expected[i]);
        i++;
    }

    EXPECT_EQ(i, 3);
}

TEST(EnumeratorTest, CppRangeBasedForLoop_EmptySequence) {
    MutableArraySequence<int> seq;

    int iterations = 0;

    for (const auto& item : seq) {
        iterations++;
    }

    EXPECT_EQ(iterations, 0);
}