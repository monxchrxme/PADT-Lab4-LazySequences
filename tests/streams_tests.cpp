#include <gtest/gtest.h>
#include "LazyContext.hpp"
#include "CollectionGenerator.hpp"

#include "GeneratorReadStream.hpp"
#include "SequenceReadStream.hpp"
#include "SequenceWriteStream.hpp"
#include "ReadWriteStream.hpp"
#include "FunctionGenerator.hpp"
#include "LazySequence.hpp"


// Поток из Генератора 
TEST(StreamsTest, GeneratorReadStreamWorks) {
    LazyContext ctx;
    
    MutableArraySequence<int> data;
    data.append(10); 
    data.append(20); 
    data.append(30);

    auto* gen = ctx.Allocate<CollectionGenerator<int>>(data);
    
    GeneratorReadStream<int> stream(gen);

    stream.open();
    EXPECT_FALSE(stream.is_end_of_stream());
    EXPECT_EQ(stream.read(), 10);
    EXPECT_EQ(stream.read(), 20);
    EXPECT_EQ(stream.read(), 30);
    EXPECT_TRUE(stream.is_end_of_stream());
    EXPECT_THROW(stream.read(), EndOfStreamException);
    
    // Проверяем контракт
    EXPECT_FALSE(stream.is_can_seek());
    EXPECT_THROW(stream.seek(0), std::logic_error);

    stream.close();
}

// Поток из Коллекции
TEST(StreamsTest, SequenceReadStreamWorks) {
    MutableArraySequence<int> data;
    data.append(100); 
    data.append(200); 
    data.append(300);

    SequenceReadStream<int> stream(&data);
    
    stream.open();
    EXPECT_EQ(stream.read(), 100); // Прочитали нулевой элемент
    EXPECT_EQ(stream.get_position(), 1);

    // Возвращаемся на 0 (проверяем перемотку)
    EXPECT_TRUE(stream.is_can_seek());
    stream.seek(0);
    EXPECT_EQ(stream.get_position(), 0);
    EXPECT_EQ(stream.read(), 100); // Снова 100

    // Прыгаем в конец
    stream.seek(2);
    EXPECT_EQ(stream.read(), 300);
    EXPECT_TRUE(stream.is_end_of_stream());

    stream.close();
}

// Смешанный поток 
TEST(StreamsTest, ReadWriteStreamWorks) {
    LazyContext ctx;
    
    // Источник
    MutableArraySequence<int> input_data;
    input_data.append(1); 
    input_data.append(2); 
    input_data.append(3);

    auto* gen = ctx.Allocate<CollectionGenerator<int>>(input_data);
    
    // Ридер и Райтер 
    GeneratorReadStream<int> reader(gen);
    MutableArraySequence<int> output_data;
    SequenceWriteStream<int> writer(&output_data);

    // Смешанный поток (принимает интерфейсы)
    ReadWriteStream<int> stream(&reader, &writer);

    stream.open();
    while (!stream.is_end_of_read_stream()) {
        int val = stream.read();
        stream.write(val * 10);
    }
    stream.close();

    // Проверяем запись
    EXPECT_EQ(output_data.get_length(), 3);
    EXPECT_EQ(output_data.get(0), 10);
    EXPECT_EQ(output_data.get(2), 30);
}

TEST(StreamsEdgeCases, ClosedStreamOperationsThrow) {
    LazyContext ctx;
    MutableArraySequence<int> data; data.append(1);
    
    // Тест закрытого ридера
    auto* gen = ctx.Allocate<CollectionGenerator<int>>(data);
    GeneratorReadStream<int> reader(gen);
    // НЕ вызываем open()
    EXPECT_TRUE(reader.is_end_of_stream()); // Закрытый поток выглядит как исчерпанный
    EXPECT_THROW(reader.read(), std::logic_error);

    // Тест закрытого райтера
    SequenceWriteStream<int> writer(&data);
    // НЕ вызываем open()
    EXPECT_THROW(writer.write(99), std::logic_error);
}

// Правило для бесконечного генератора
Option<int> InfRule(int index) { return Option<int>(index); }

TEST(StreamsEdgeCases, SequenceReadStreamOnInfiniteSequence) {
    LazyContext ctx;
    auto* gen = ctx.Allocate<FunctionGenerator<int>>(InfRule); 
    auto* lazy_seq = ctx.Allocate<LazySequence<int>>(gen, &ctx);

    SequenceReadStream<int> stream(lazy_seq);
    stream.open();

    // Метод is_end_of_stream должен понять, что последовательность
    // бесконечна (поймав logic_error от get_length) и вернуть false
    EXPECT_FALSE(stream.is_end_of_stream());
    
    EXPECT_EQ(stream.read(), 0);
    EXPECT_EQ(stream.read(), 1);
    EXPECT_FALSE(stream.is_end_of_stream()); 
    
    stream.close();
}

TEST(StreamsEdgeCases, SequenceWriteStreamIsolated) {
    MutableArraySequence<std::string> buffer;
    SequenceWriteStream<std::string> writer(&buffer);

    writer.open();
    EXPECT_EQ(writer.get_position(), 0);
    
    size_t new_pos = writer.write("Test1");
    EXPECT_EQ(new_pos, 1);
    EXPECT_EQ(writer.get_position(), 1);
    
    writer.write("Test2");
    writer.close();

    EXPECT_EQ(buffer.get_length(), 2);
    EXPECT_EQ(buffer.get(0), "Test1");
    EXPECT_EQ(buffer.get(1), "Test2");
}