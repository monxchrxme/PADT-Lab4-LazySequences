#include <gtest/gtest.h>
#include "LazyContext.hpp"
#include "CollectionGenerator.hpp"
#include "ReadOnlyStream.hpp"
#include "WriteOnlyStream.hpp"
#include "ReadWriteStream.hpp"

TEST(StreamsTest, ReadOnlyStreamWorks) {
    LazyContext ctx;
    
    // Подготавливаем данные
    MutableArraySequence<int> data;
    data.append(10);
    data.append(20);
    data.append(30);

    // Создаем генератор и направляем в него поток
    auto* gen = ctx.Allocate<CollectionGenerator<int>>(data);
    ReadOnlyStream<int> stream(gen);

    // Поток закрыт по умолчанию
    EXPECT_TRUE(stream.is_end_of_stream());
    EXPECT_THROW(stream.read(), std::logic_error);

    stream.open();

    EXPECT_FALSE(stream.is_end_of_stream());
    EXPECT_EQ(stream.get_position(), 0);

    // Читаем первый элемент
    EXPECT_EQ(stream.read(), 10);
    EXPECT_EQ(stream.get_position(), 1);

    // Читаем остальные
    EXPECT_EQ(stream.read(), 20);
    EXPECT_EQ(stream.read(), 30);

    // Поток должен закончиться
    EXPECT_TRUE(stream.is_end_of_stream());
    EXPECT_EQ(stream.get_position(), 3);
    
    // Попытка прочитать за границей вызывает EndOfStreamException
    EXPECT_THROW(stream.read(), EndOfStreamException);

    stream.close();
}

TEST(StreamsTest, WriteOnlyStreamWorks) {
    MutableArraySequence<std::string> output_buffer;
    WriteOnlyStream<std::string> stream(&output_buffer);

    stream.open();

    EXPECT_EQ(stream.write("Hello"), 1);
    EXPECT_EQ(stream.write("FSM"), 2);
    EXPECT_EQ(stream.write("World"), 3);

    stream.close();

    // Проверяем, что данные реально попали в коллекцию
    EXPECT_EQ(output_buffer.get_length(), 3);
    EXPECT_EQ(output_buffer.get(0), "Hello");
    EXPECT_EQ(output_buffer.get(1), "FSM");
    EXPECT_EQ(output_buffer.get(2), "World");
}

TEST(StreamsTest, ReadWriteStreamWorks) {
    LazyContext ctx;
    
    // 1. Подготавливаем источник данных (для чтения)
    MutableArraySequence<int> input_data;
    input_data.append(1);
    input_data.append(2);
    input_data.append(3);
    auto* gen = ctx.Allocate<CollectionGenerator<int>>(input_data);

    // 2. Подготавливаем приемник данных (для записи)
    MutableArraySequence<int> output_data;

    // 3. Создаем смешанный поток
    ReadWriteStream<int> stream(gen, &output_data);

    // Поток закрыт
    EXPECT_TRUE(stream.is_end_of_read_stream());

    stream.open();

    // 4. Эмулируем логику: читаем, умножаем на 10, записываем
    while (!stream.is_end_of_read_stream()) {
        int val = stream.read();
        stream.write(val * 10);
    }

    stream.close();

    // 5. Проверяем, что в приемник записались правильные данные
    EXPECT_EQ(output_data.get_length(), 3);
    EXPECT_EQ(output_data.get(0), 10);
    EXPECT_EQ(output_data.get(1), 20);
    EXPECT_EQ(output_data.get(2), 30);
}