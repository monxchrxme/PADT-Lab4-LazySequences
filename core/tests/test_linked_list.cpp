#include <gtest/gtest.h>
#include <stdexcept>
#include "../src/memory/linked_list.hpp"
#include "../src/types/exceptions.hpp"

// constructors test

TEST(LinkedListTest, DefaultConstructor) {
    LinkedList<int> list;
    EXPECT_EQ(list.get_length(), 0);
    EXPECT_THROW(list.get_first(), IndexOutOfRange);
    EXPECT_THROW(list.get_last(), IndexOutOfRange);
}

TEST(LinkedListTest, ArrayConstructor) {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);
    EXPECT_EQ(list.get_length(), 3);
    EXPECT_EQ(list.get_first(), 1);
    EXPECT_EQ(list.get_last(), 3);
    EXPECT_EQ(list.get(1), 2);

    EXPECT_THROW(LinkedList<int>(data, -1), std::invalid_argument);
}

// testing the Rule of Five

TEST(LinkedListTest, CopyAndMoveConstructors) {
    int data[] = {10, 20};
    LinkedList<int> original(data, 2);

    // copy
    LinkedList<int> copy(original);
    EXPECT_EQ(copy.get_length(), 2);
    copy.append(30);
    EXPECT_EQ(original.get_length(), 2); // original has not changed

    // move
    LinkedList<int> moved(static_cast<LinkedList<int>&&>(original));
    EXPECT_EQ(moved.get_length(), 2);
    EXPECT_EQ(moved.get_first(), 10);
    EXPECT_EQ(original.get_length(), 0); // original is empty
}

// modifying operations test

TEST(LinkedListTest, AppendAndPrepend) {
    LinkedList<int> list;
    list.append(20);
    list.prepend(10);
    list.append(30);

    // structure: 10 -> 20 -> 30
    EXPECT_EQ(list.get_length(), 3);
    EXPECT_EQ(list.get_first(), 10);
    EXPECT_EQ(list.get_last(), 30);
    EXPECT_EQ(list.get(1), 20);
}

TEST(LinkedListTest, InsertAt) {
    LinkedList<int> list;
    list.insert_at(20, 0); // to an empty list
    list.insert_at(10, 0); // to the top
    list.insert_at(30, 2); // to the end
    list.insert_at(25, 2); // in the middle

    // structure: 10 -> 20 -> 25 -> 30
    EXPECT_EQ(list.get_length(), 4);
    EXPECT_EQ(list.get(2), 25);

    EXPECT_THROW(list.insert_at(99, 100), IndexOutOfRange);
}

TEST(LinkedListTest, RemoveAt) {
    int data[] = {1, 2, 3, 4};
    LinkedList<int> list(data, 4);

    list.remove_at(2);
    EXPECT_EQ(list.get_length(), 3);
    EXPECT_EQ(list.get(2), 4);

    list.remove_at(0); // delete head (1)
    EXPECT_EQ(list.get_first(), 2);

    list.remove_at(1); // delete tail (4)
    EXPECT_EQ(list.get_last(), 2);

    list.remove_at(0); // delete lest elem
    EXPECT_EQ(list.get_length(), 0);
    EXPECT_THROW(list.get_first(), IndexOutOfRange);
}

// getters test

TEST(LinkedListTest, GetSubList) {
    int data[] = {10, 20, 30, 40, 50};
    LinkedList<int> list(data, 5);

    LinkedList<int>* sub = list.get_sub_list(1, 3); // 20, 30, 40
    EXPECT_EQ(sub->get_length(), 3);
    EXPECT_EQ(sub->get_first(), 20);
    EXPECT_EQ(sub->get_last(), 40);

    delete sub;

    EXPECT_THROW(list.get_sub_list(3, 1), IndexOutOfRange);
    EXPECT_THROW(list.get_sub_list(-1, 2), IndexOutOfRange);
}

TEST(LinkedListTest, Concat) {
    int data1[] = {1, 2};
    int data2[] = {3, 4};
    LinkedList<int> list1(data1, 2);
    LinkedList<int> list2(data2, 2);

    LinkedList<int>* result = list1.concat(&list2);
    EXPECT_EQ(result->get_length(), 4);
    EXPECT_EQ(result->get(2), 3);

    delete result;
}