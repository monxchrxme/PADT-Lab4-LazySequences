#include <gtest/gtest.h>
#include "Cardinal.hpp"

// ТЕСТЫ ДЛЯ ТРАНСФИНИТНЫХ ЧИСЕЛ (Cardinal) 

// 1. Проверка создания конечных чисел
TEST(CardinalTest, FiniteConstruction) {
    Cardinal zero; // По умолчанию 0
    EXPECT_FALSE(zero.is_infinite());
    EXPECT_EQ(zero.get_value(), 0);

    Cardinal positive(42);
    EXPECT_FALSE(positive.is_infinite());
    EXPECT_EQ(positive.get_value(), 42);
}

// 2. Проверка создания бесконечности
TEST(CardinalTest, InfiniteConstruction) {
    Cardinal inf = Cardinal::Infinity();
    EXPECT_TRUE(inf.is_infinite());
}

// 3. Проверка выброса исключений (негативные сценарии)
TEST(CardinalTest, ExceptionHandling) {
    // Кардинальное число не может быть отрицательным (это длина)
    EXPECT_THROW(Cardinal(-1), std::invalid_argument);
    EXPECT_THROW(Cardinal(-999), std::invalid_argument);

    // Нельзя получить int из бесконечности
    Cardinal inf = Cardinal::Infinity();
    EXPECT_THROW(inf.get_value(), std::logic_error);
}

// 4. Проверка математики: Сложение
TEST(CardinalTest, AdditionLogic) {
    Cardinal c1(10);
    Cardinal c2(20);
    Cardinal inf = Cardinal::Infinity();

    // Конечное + Конечное = Конечное
    Cardinal sum = c1 + c2;
    EXPECT_FALSE(sum.is_infinite());
    EXPECT_EQ(sum.get_value(), 30);

    // Конечное + Бесконечность = Бесконечность
    EXPECT_TRUE((c1 + inf).is_infinite());
    EXPECT_TRUE((inf + c1).is_infinite());

    // Бесконечность + Бесконечность = Бесконечность
    EXPECT_TRUE((inf + inf).is_infinite());
}

// 5. Проверка логических операторов сравнения
TEST(CardinalTest, ComparisonOperators) {
    Cardinal a(10);
    Cardinal b(10);
    Cardinal c(15);
    Cardinal inf1 = Cardinal::Infinity();
    Cardinal inf2 = Cardinal::Infinity();

    // Сравнение конечных
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);

    // Сравнение бесконечностей (Бесконечность == Бесконечность)
    EXPECT_TRUE(inf1 == inf2);
    EXPECT_FALSE(inf1 != inf2);

    // Сравнение конечного с бесконечностью
    EXPECT_FALSE(a == inf1);
    EXPECT_TRUE(a != inf1);
}

// 6. Проверка конвертации в строку (для UI)
TEST(CardinalTest, StringRepresentation) {
    Cardinal c(1337);
    Cardinal inf = Cardinal::Infinity();

    EXPECT_EQ(c.to_string(), "1337");
    EXPECT_EQ(inf.to_string(), "Infinity");
}