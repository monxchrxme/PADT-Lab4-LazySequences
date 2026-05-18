#include <gtest/gtest.h>
#include <stdexcept>
#include "../src/memory/dynamic_array.hpp"
#include "../src/types/exceptions.hpp"

// constructors test

TEST(DynamicArrayTest, DefaultConstructor) {
    DynamicArray<int> arr;
    EXPECT_EQ(arr.get_size(), 0);
    EXPECT_EQ(arr.get_capacity(), 0);
    EXPECT_THROW(arr.get(0), IndexOutOfRange);
}

TEST(DynamicArrayTest, SizeConstructor) {
    DynamicArray<int> arr(5);
    EXPECT_EQ(arr.get_size(), 5);
    EXPECT_EQ(arr.get_capacity(), 5);
    // value-initialization test (have to be 0's)
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(arr.get(i), 0);
    }
    EXPECT_THROW(DynamicArray<int>(-1), std::invalid_argument);
}

TEST(DynamicArrayTest, ArrayConstructor) {
    int data[] = {10, 20, 30};
    DynamicArray<int> arr(data, 3);
    EXPECT_EQ(arr.get_size(), 3);
    EXPECT_EQ(arr.get(0), 10);
    EXPECT_EQ(arr.get(2), 30);
    EXPECT_THROW(DynamicArray<int>(data, -1), std::invalid_argument);
    EXPECT_THROW(DynamicArray<int>(nullptr, 5), std::invalid_argument);
}

// testing the Rule of Five (Copy and Move)

TEST(DynamicArrayTest, CopyConstructor) {
    int data[] = {1, 2, 3};
    DynamicArray<int> original(data, 3);
    DynamicArray<int> copy(original);

    EXPECT_EQ(copy.get_size(), 3);
    EXPECT_EQ(copy.get(1), 2);

    // deep copy test (changing the copy does not affect the original)
    copy.set(1, 99);
    EXPECT_EQ(original.get(1), 2);
    EXPECT_EQ(copy.get(1), 99);
}

TEST(DynamicArrayTest, MoveConstructor) {
    int data[] = {1, 2, 3};
    DynamicArray<int> original(data, 3);
    DynamicArray<int> moved(static_cast<DynamicArray<int>&&>(original)); // imitation std::move

    EXPECT_EQ(moved.get_size(), 3);
    EXPECT_EQ(moved.get(0), 1);

    // the original must remain empty (noexcept relocation guarantee)
    EXPECT_EQ(original.get_size(), 0);
    EXPECT_EQ(original.get_capacity(), 0);
}

TEST(DynamicArrayTest, AssignmentOperators) {
    int data1[] = {1, 2};
    int data2[] = {3, 4, 5};
    DynamicArray<int> arr1(data1, 2);
    DynamicArray<int> arr2(data2, 3);

    // copy assignment
    arr1 = arr2;
    EXPECT_EQ(arr1.get_size(), 3);
    EXPECT_EQ(arr1.get(2), 5);

    // self-assignment (shouldn't break anything)
    arr1 = arr1;
    EXPECT_EQ(arr1.get_size(), 3);

    // move assignment
    DynamicArray<int> arr3;
    arr3 = static_cast<DynamicArray<int>&&>(arr1);
    EXPECT_EQ(arr3.get_size(), 3);
    EXPECT_EQ(arr1.get_size(), 0);
}

// modifying operations test

TEST(DynamicArrayTest, ResizeLogic) {
    DynamicArray<int> arr(2);
    arr.set(0, 10);
    arr.set(1, 20);

    // increasing (reallocation)
    arr.resize(5);
    EXPECT_EQ(arr.get_size(), 5);
    EXPECT_GE(arr.get_capacity(), 5);
    EXPECT_EQ(arr.get(0), 10); // old data ok

    // decreasing (wo reallocation)
    int old_cap = arr.get_capacity();
    arr.resize(1);
    EXPECT_EQ(arr.get_size(), 1);
    EXPECT_EQ(arr.get_capacity(), old_cap);

    // complete cleaning
    arr.resize(0);
    EXPECT_EQ(arr.get_size(), 0);
    EXPECT_EQ(arr.get_capacity(), 0);
}

TEST(DynamicArrayTest, RemoveAt) {
    int data[] = {10, 20, 30, 40};
    DynamicArray<int> arr(data, 4);

    arr.remove_at(1);
    EXPECT_EQ(arr.get_size(), 3);
    EXPECT_EQ(arr.get(0), 10);
    EXPECT_EQ(arr.get(1), 30);
    EXPECT_EQ(arr.get(2), 40);

    EXPECT_THROW(arr.remove_at(10), IndexOutOfRange);
    EXPECT_THROW(arr.remove_at(-1), IndexOutOfRange);
}