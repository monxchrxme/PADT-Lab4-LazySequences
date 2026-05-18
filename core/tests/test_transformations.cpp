#include <gtest/gtest.h>
#include "../src/sequences/mutable_array_sequence.hpp"
#include "../src/sequences/immutable_array_sequence.hpp"
#include "../src/sequences/mutable_list_sequence.hpp"
#include "../src/sequences/algorithms.hpp"
#include "../src/types/pair.hpp"
#include "../src/types/exceptions.hpp"

// complex test for slice

TEST(TransformationsTest, Slice_Mutable_PdfExample) {
    int data[] = {1, 2, 3, 4, 5};
    MutableArraySequence<int> seq(data, 5);

    int insert_data[] = {9, 10};
    MutableArraySequence<int> insert_seq(insert_data, 2);

    Sequence<int>* result = seq.slice(1, 2, &insert_seq);

    EXPECT_EQ(result->get_length(), 5);
    EXPECT_EQ(result->get(1), 9);
    EXPECT_EQ(result->get(2), 10);
    EXPECT_EQ(result->get(3), 4);

    delete result;
}

TEST(TransformationsTest, Slice_Immutable_ReturnsImmutable) {
    int data[] = {10, 20, 30};
    ImmutableArraySequence<int> seq(data, 3);

    // cut out the middle element without the insert
    Sequence<int>* result = seq.slice(1, 1);

    EXPECT_EQ(result->get_length(), 2);
    EXPECT_EQ(result->get(0), 10);
    EXPECT_EQ(result->get(1), 30);

    // verify that the result remains immutable
    EXPECT_NE(dynamic_cast<ImmutableArraySequence<int>*>(result), nullptr);

    // original collection should not have changed
    EXPECT_EQ(seq.get_length(), 3);

    delete result;
}

TEST(TransformationsTest, Slice_NegativeIndexAndOutOfBounds) {
    int data[] = {10, 20, 30, 40};
    MutableListSequence<int> seq(data, 4);

    // -2 means starting from element 30. We extract 2 elements from the end
    Sequence<int>* result = seq.slice(-2, 2);

    EXPECT_EQ(result->get_length(), 2);
    EXPECT_EQ(result->get(0), 10);
    EXPECT_EQ(result->get(1), 20);

    delete result;

    EXPECT_THROW(seq.slice(10, 1), IndexOutOfRange);
    EXPECT_THROW(seq.slice(-10, 1), IndexOutOfRange);
}

// complex test for split

bool is_even(const int& x) { return x % 2 == 0; }
bool is_zero(const int& x) { return x == 0; }

TEST(TransformationsTest, Split_Mutable_ByCondition) {
    int data[] = {1, 3, 2, 5, 7, 4, 9};
    MutableArraySequence<int> seq(data, 7);

    Sequence<Sequence<int>*>* result = Split<int>(&seq, is_even);

    ASSERT_EQ(result->get_length(), 3); // 3 chunks: {1,3}, {5,7}, {9}

    EXPECT_EQ(result->get(0)->get_length(), 2); // {1, 3}
    EXPECT_EQ(result->get(1)->get_length(), 2); // {5, 7}
    EXPECT_EQ(result->get(2)->get_length(), 1); // {9}

    for(int i = 0; i < result->get_length(); ++i) delete result->get(i);
    delete result;
}

TEST(TransformationsTest, Split_Immutable_MaintainsPurity) {
    int data[] = {0, 1, 0, 2};
    ImmutableArraySequence<int> seq(data, 4);

    Sequence<Sequence<int>*>* result = Split<int>(&seq, is_zero);

    // separator '0' appears at the beginning, which means the first chunk is an empty array
    ASSERT_EQ(result->get_length(), 3); // {}, {1}, {2}

    EXPECT_EQ(result->get(0)->get_length(), 0);
    EXPECT_EQ(result->get(1)->get_length(), 1);
    EXPECT_EQ(result->get(2)->get_length(), 1);

    // internal containers must also be immutable
    EXPECT_NE(dynamic_cast<ImmutableArraySequence<int>*>(result->get(1)), nullptr);

    for(int i = 0; i < result->get_length(); ++i) delete result->get(i);
    delete result;
}

// zip test

TEST(TransformationsTest, Zip_EqualLengths) {
    int data1[] = {1, 2, 3};
    char data2[] = {'a', 'b', 'c'};
    MutableArraySequence<int> seq1(data1, 3);
    MutableArraySequence<char> seq2(data2, 3);

    Sequence<Pair<int, char>>* zipped = Zip<int, char>(&seq1, &seq2);

    ASSERT_EQ(zipped->get_length(), 3);
    EXPECT_EQ(zipped->get(0).first, 1);
    EXPECT_EQ(zipped->get(0).second, 'a');
    EXPECT_EQ(zipped->get(2).first, 3);
    EXPECT_EQ(zipped->get(2).second, 'c');

    delete zipped;
}

TEST(TransformationsTest, Zip_DifferentLengths_FirstShorter) {
    int data1[] = {1};
    double data2[] = {1.1, 2.2, 3.3};
    MutableArraySequence<int> seq1(data1, 1);
    MutableArraySequence<double> seq2(data2, 3);

    Sequence<Pair<int, double>>* zipped = Zip<int, double>(&seq1, &seq2);

    ASSERT_EQ(zipped->get_length(), 1);
    EXPECT_EQ(zipped->get(0).first, 1);
    EXPECT_DOUBLE_EQ(zipped->get(0).second, 1.1);

    delete zipped;
}

TEST(TransformationsTest, Zip_DifferentLengths_SecondShorter) {
    int data1[] = {1, 2, 3};
    double data2[] = {1.1};
    MutableArraySequence<int> seq1(data1, 3);
    MutableArraySequence<double> seq2(data2, 1);

    Sequence<Pair<int, double>>* zipped = Zip<int, double>(&seq1, &seq2);

    ASSERT_EQ(zipped->get_length(), 1);
    EXPECT_EQ(zipped->get(0).first, 1);
    EXPECT_DOUBLE_EQ(zipped->get(0).second, 1.1);

    delete zipped;
}

TEST(TransformationsTest, Zip_EmptySequences) {
    MutableArraySequence<int> empty1;
    MutableListSequence<char> empty2;

    Sequence<Pair<int, char>>* zipped = Zip<int, char>(&empty1, &empty2);

    ASSERT_EQ(zipped->get_length(), 0);

    delete zipped;
}

TEST(TransformationsTest, Zip_PolymorphicTypes) {
    int data1[] = {10, 20};
    char data2[] = {'x', 'y'};

    ImmutableArraySequence<int> arr_seq(data1, 2); // Immutable Array
    MutableListSequence<char> list_seq(data2, 2);  // Mutable List

    Sequence<Pair<int, char>>* zipped = Zip<int, char>(&arr_seq, &list_seq);

    ASSERT_EQ(zipped->get_length(), 2);
    EXPECT_EQ(zipped->get(1).first, 20);
    EXPECT_EQ(zipped->get(1).second, 'y');

    delete zipped;
}

// unzip tests

TEST(TransformationsTest, Unzip_NormalSequence) {
    Pair<int, char> data[] = { MakePair(1, 'a'), MakePair(2, 'b'), MakePair(3, 'c') };
    MutableArraySequence<Pair<int, char>> zipped(data, 3);

    Pair<Sequence<int>*, Sequence<char>*> unzipped = Unzip<int, char>(&zipped);

    Sequence<int>* seq1 = unzipped.first;
    Sequence<char>* seq2 = unzipped.second;

    ASSERT_EQ(seq1->get_length(), 3);
    ASSERT_EQ(seq2->get_length(), 3);

    EXPECT_EQ(seq1->get(0), 1);
    EXPECT_EQ(seq2->get(0), 'a');
    EXPECT_EQ(seq1->get(2), 3);
    EXPECT_EQ(seq2->get(2), 'c');

    delete seq1;
    delete seq2;
}

TEST(TransformationsTest, Unzip_EmptySequence) {
    MutableArraySequence<Pair<int, double>> empty_zipped;

    Pair<Sequence<int>*, Sequence<double>*> unzipped = Unzip<int, double>(&empty_zipped);

    Sequence<int>* seq1 = unzipped.first;
    Sequence<double>* seq2 = unzipped.second;

    ASSERT_EQ(seq1->get_length(), 0);
    ASSERT_EQ(seq2->get_length(), 0);

    delete seq1;
    delete seq2;
}