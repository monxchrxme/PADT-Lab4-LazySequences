#include <gtest/gtest.h>
#include "../src/sequences/mutable_array_sequence.hpp"
#include "../src/sequences/mutable_list_sequence.hpp"
#include "../src/types/option.hpp"

// solated tests for the Option<T> monad itself

TEST(OptionTest, StateManagement) {
    Option<int> none_opt;
    EXPECT_TRUE(none_opt.is_none());
    EXPECT_FALSE(none_opt.is_some());
    EXPECT_EQ(none_opt.unwrap_or(99), 99);

    EXPECT_THROW(none_opt.unwrap(), std::logic_error);

    Option<int> some_opt(42);
    EXPECT_TRUE(some_opt.is_some());
    EXPECT_FALSE(some_opt.is_none());
    EXPECT_EQ(some_opt.unwrap(), 42);
    EXPECT_EQ(some_opt.unwrap_or(99), 42);
}

TEST(OptionTest, StringPayload) {
    Option<std::string> opt("Hello");
    ASSERT_TRUE(opt.is_some());
    EXPECT_EQ(opt.unwrap(), "Hello");
}

// Try-semantics for ArraySequence

TEST(TrySemanticsTest, ArraySequence_Empty) {
    MutableArraySequence<int> seq;

    EXPECT_TRUE(seq.try_get_first().is_none());
    EXPECT_TRUE(seq.try_get_last().is_none());
    EXPECT_TRUE(seq.try_get(0).is_none());
    EXPECT_TRUE(seq.try_get(-1).is_none());
}

TEST(TrySemanticsTest, ArraySequence_Filled) {
    int data[] = {10, 20, 30};
    MutableArraySequence<int> seq(data, 3);

    EXPECT_EQ(seq.try_get_first().unwrap(), 10);
    EXPECT_EQ(seq.try_get_last().unwrap(), 30);

    EXPECT_EQ(seq.try_get(0).unwrap(), 10);
    EXPECT_EQ(seq.try_get(1).unwrap(), 20);
    EXPECT_EQ(seq.try_get(2).unwrap(), 30);

    EXPECT_TRUE(seq.try_get(-1).is_none());
    EXPECT_TRUE(seq.try_get(3).is_none());
    EXPECT_TRUE(seq.try_get(100).is_none());
}

// Try-semantics for ListSequence

TEST(TrySemanticsTest, ListSequence_Empty) {
    MutableListSequence<int> seq;

    EXPECT_TRUE(seq.try_get_first().is_none());
    EXPECT_TRUE(seq.try_get_last().is_none());
    EXPECT_TRUE(seq.try_get(0).is_none());
    EXPECT_TRUE(seq.try_get(-1).is_none());
}

TEST(TrySemanticsTest, ListSequence_SingleElement) {
    int data[] = {42};
    MutableListSequence<int> seq(data, 1);

    EXPECT_EQ(seq.try_get_first().unwrap(), 42);
    EXPECT_EQ(seq.try_get_last().unwrap(), 42);
    EXPECT_EQ(seq.try_get(0).unwrap(), 42);

    EXPECT_TRUE(seq.try_get(-1).is_none());
    EXPECT_TRUE(seq.try_get(1).is_none());
}

TEST(TrySemanticsTest, ListSequence_Filled) {
    int data[] = {100, 200, 300, 400};
    MutableListSequence<int> seq(data, 4);

    EXPECT_EQ(seq.try_get_first().unwrap(), 100);
    EXPECT_EQ(seq.try_get_last().unwrap(), 400);
    EXPECT_EQ(seq.try_get(2).unwrap(), 300);

    EXPECT_TRUE(seq.try_get(-5).is_none());
    EXPECT_TRUE(seq.try_get(4).is_none());
}

// polymorphic call (sequence interface)

TEST(TrySemanticsTest, PolymorphicAccess) {
    int data[] = {7, 8, 9};
    MutableListSequence<int> list_seq(data, 3);

    // work through an abstract base class pointer
    const Sequence<int>* base_ptr = &list_seq;

    // verify that virtual methods correctly route the call
    Option<int> opt = base_ptr->try_get(1);
    ASSERT_TRUE(opt.is_some());
    EXPECT_EQ(opt.unwrap(), 8);

    Option<int> bad_opt = base_ptr->try_get(99);
    EXPECT_TRUE(bad_opt.is_none());
}