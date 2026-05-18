#include <gtest/gtest.h>
#include "../src/sequences/mutable_array_sequence.hpp"
#include "../src/sequences/mutable_list_sequence.hpp"
#include "../src/sequences/immutable_array_sequence.hpp"
#include "../src/sequences/immutable_list_sequence.hpp"

// Builder test for Mutable collections

TEST(BuilderTest, MutableArrayBuilder) {
    auto builder = new MutableArraySequence<int>::Builder();

    builder->append(10);
    builder->append(20);
    builder->append(30);

    Sequence<int>* result = builder->build();

    EXPECT_EQ(result->get_length(), 3);
    EXPECT_EQ(result->get(0), 10);
    EXPECT_EQ(result->get(2), 30);

    delete result;
    delete builder;
}

TEST(BuilderTest, MutableListBuilder) {
    auto builder = new MutableListSequence<int>::Builder();

    builder->append(100);
    builder->append(200);

    Sequence<int>* result = builder->build();

    EXPECT_EQ(result->get_length(), 2);
    EXPECT_EQ(result->get(0), 100);
    EXPECT_EQ(result->get(1), 200);

    delete result;
    delete builder;
}

// Builder test for Immutable collections

TEST(BuilderTest, ImmutableArrayBuilder) {
    auto builder = new ImmutableArraySequence<int>::Builder();

    builder->append(1);
    builder->append(2);

    Sequence<int>* result = builder->build();

    EXPECT_EQ(result->get_length(), 2);
    EXPECT_EQ(result->get(0), 1);

    // verify that an Immutable object specifically has been created
    EXPECT_NE(dynamic_cast<ImmutableArraySequence<int>*>(result), nullptr);

    delete result;
    delete builder;
}

TEST(BuilderTest, ImmutableListBuilder) {
    auto builder = new ImmutableListSequence<int>::Builder();

    builder->append(42);

    Sequence<int>* result = builder->build();

    EXPECT_EQ(result->get_length(), 1);
    EXPECT_EQ(result->get(0), 42);
    EXPECT_NE(dynamic_cast<ImmutableListSequence<int>*>(result), nullptr);

    delete result;
    delete builder;
}