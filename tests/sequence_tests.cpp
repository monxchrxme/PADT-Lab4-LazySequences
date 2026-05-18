#include <gtest/gtest.h>
#include "LazyContext.hpp"
#include "LazySequence.hpp"
#include "FunctionGenerator.hpp"
#include "FilterGenerator.hpp"
#include "RecurrentGenerator.hpp"
#include "mutable_array_sequence.hpp"


static int g_formula_calls = 0;

// Правило: i -> i * 10
Option<int> MultiplierRule(int index) {
    g_formula_calls++;
    return Option<int>(index * 10); 
}

// Правило для фильтра: берем только те числа, которые делятся на 20
bool DivisibleBy20(const int& value) {
    return value % 20 == 0;
}

// Правило: Конечный генератор (выдает только 3 числа: 0, 1, 4)
Option<int> FiniteSquareRule(int index) {
    if (index >= 3) {
        return Option<int>(); 
    }
    return Option<int>(index * index);
}

// Правило: 0, 1, 2, 3, 4, 5...
Option<int> SequentialRule(int index) {
    return Option<int>(index);
}


TEST(LazySequenceTest, MemoizationWorks) {
    g_formula_calls = 0;
    LazyContext ctx;

    auto* gen = ctx.Allocate<FunctionGenerator<int>>(MultiplierRule);
    auto* seq = ctx.Allocate<LazySequence<int>>(gen, &ctx);

    // 1. Сначала элементов нет
    EXPECT_EQ(seq->get_materialized_count(), 0);

    // 2. Запрашиваем 2-й элемент (индексы: 0, 1, 2)
    // Должны вычислиться числа: 0, 10, 20
    EXPECT_EQ(seq->get(2), 20);
    EXPECT_EQ(g_formula_calls, 3); 
    EXPECT_EQ(seq->get_materialized_count(), 3);

    // 3. ПРОВЕРКА МЕМОИЗАЦИИ: снова запрашиваем 2-й элемент
    EXPECT_EQ(seq->get(2), 20);
    EXPECT_EQ(seq->get(1), 10);
    
    // Формула БОЛЬШЕ НЕ ВЫЗЫВАЛАСЬ
    EXPECT_EQ(g_formula_calls, 3); 
}

TEST(LazySequenceTest, InfiniteCardinality) {
    LazyContext ctx;
    auto* gen = ctx.Allocate<FunctionGenerator<int>>(MultiplierRule); // По умолчанию Infinity
    auto* seq = ctx.Allocate<LazySequence<int>>(gen, &ctx);

    EXPECT_TRUE(seq->get_cardinality().is_infinite());
    
    // Попытка получить int длину бесконечного списка должна кидать исключение, 
    // чтобы программа не зависла
    EXPECT_THROW(seq->get_length(), std::logic_error);
}

TEST(LazySequenceTest, LazyWhereOperation) {
    LazyContext ctx;

    // Исходная последовательность: 0, 10, 20, 30, 40, 50...
    auto* gen = ctx.Allocate<FunctionGenerator<int>>(MultiplierRule);
    auto* seq = ctx.Allocate<LazySequence<int>>(gen, &ctx);

    // Применяем ленивый фильтр (должно остаться: 0, 20, 40...)
    Sequence<int>* filtered_seq = seq->where(DivisibleBy20);

    // Проверяем индексы нового ленивого списка
    EXPECT_EQ(filtered_seq->get(0), 0);
    EXPECT_EQ(filtered_seq->get(1), 20);
    EXPECT_EQ(filtered_seq->get(2), 40);

    // Проверяем, что исходный список остался неизменным 
    EXPECT_EQ(seq->get(1), 10);
    EXPECT_EQ(seq->get(2), 20);
}

TEST(LazySequenceTest, FunctionGeneratorFiniteSequence) {
    LazyContext ctx;

    // Передаем правило и явно указываем кардинальность = 3
    auto* gen = ctx.Allocate<FunctionGenerator<int>>(FiniteSquareRule, Cardinal(3));
    auto* seq = ctx.Allocate<LazySequence<int>>(gen, &ctx);

    // Проверяем классические методы коллекции
    EXPECT_EQ(seq->get_first(), 0);
    
    // get_last() заставит ленивый список вычислиться до конца
    EXPECT_EQ(seq->get_last(), 4); 
    
    // Длина должна быть 3
    EXPECT_EQ(seq->get_length(), 3);

    // Попытка выйти за пределы конечного ленивого списка
    EXPECT_THROW(seq->get(3), std::out_of_range);
    EXPECT_THROW(seq->get(10), std::out_of_range);
}

// Правило для Фибоначчи: берем сумму двух элементов окна
Option<int> FibonacciRule(const Sequence<int>& window) {
    // Убеждаемся, что в окне ровно 2 элемента
    if (window.get_length() != 2) {
        return Option<int>(); // Ошибка окна, возвращаем None
    }
    
    // F(n) = F(n-1) + F(n-2)
    int next_val = window.get(0) + window.get(1);
    
    return Option<int>(next_val);
}

TEST(LazySequenceTest, RecurrentGeneratorFibonacci) {
    LazyContext ctx;

    // 1. Создаем стартовое окно [0, 1] (первые два числа Фибоначчи)
    MutableArraySequence<int> initial_window;
    initial_window.append(0);
    initial_window.append(1);

    // 2. Аллоцируем рекуррентный генератор
    auto* gen = ctx.Allocate<RecurrentGenerator<int>>(&initial_window, FibonacciRule);
    
    // 3. Создаем ленивый список
    auto* fib_seq = ctx.Allocate<LazySequence<int>>(gen, &ctx);

    // Проверяем классический ряд Фибоначчи: 0, 1, 1, 2, 3, 5, 8, 13 и тд 
    EXPECT_EQ(fib_seq->get(0), 0);
    EXPECT_EQ(fib_seq->get(1), 1);
    EXPECT_EQ(fib_seq->get(2), 1);
    EXPECT_EQ(fib_seq->get(3), 2);
    EXPECT_EQ(fib_seq->get(4), 3);
    EXPECT_EQ(fib_seq->get(5), 5);
    EXPECT_EQ(fib_seq->get(6), 8);
    EXPECT_EQ(fib_seq->get(7), 13);
    
    EXPECT_EQ(fib_seq->get(15), 610);

    EXPECT_EQ(fib_seq->get(7), 13);
}


TEST(LazyOperationsTest, SliceAndSubsequence) {
    LazyContext ctx;
    auto* gen = ctx.Allocate<FunctionGenerator<int>>(SequentialRule);
    auto* seq = ctx.Allocate<LazySequence<int>>(gen, &ctx);

    // 1. Slice (с 2-го индекса берем 3 элемента: 2, 3, 4)
    Sequence<int>* sliced = seq->slice(2, 3);
    EXPECT_EQ(sliced->get_length(), 3);
    EXPECT_EQ(sliced->get(0), 2);
    EXPECT_EQ(sliced->get(2), 4);

    // 2. Subsequence (от 1 до 3: 1, 2, 3)
    Sequence<int>* subseq = seq->get_subsequence(1, 3);
    EXPECT_EQ(subseq->get_length(), 3);
    EXPECT_EQ(subseq->get(0), 1);
    EXPECT_EQ(subseq->get(2), 3);
    
    // 3. Create Empty
    Sequence<int>* empty_seq = seq->create_empty();
    EXPECT_EQ(empty_seq->get_length(), 0);
    EXPECT_THROW(empty_seq->get_first(), std::out_of_range);

    // 4. Clone
    Sequence<int>* cloned_seq = sliced->clone();
    EXPECT_EQ(cloned_seq->get_length(), 3);
    EXPECT_EQ(cloned_seq->get(0), 2);
}

TEST(LazyOperationsTest, ConcatOperation) {
    LazyContext ctx;
    
    // Создаем два конечных списка: [0, 1] и [0, 1, 2]
    auto* gen1 = ctx.Allocate<FunctionGenerator<int>>(SequentialRule, Cardinal(2));
    auto* seq1 = ctx.Allocate<LazySequence<int>>(gen1, &ctx);
    
    auto* gen2 = ctx.Allocate<FunctionGenerator<int>>(SequentialRule, Cardinal(3));
    auto* seq2 = ctx.Allocate<LazySequence<int>>(gen2, &ctx);

    // Склеиваем
    Sequence<int>* concatenated = seq1->concat(seq2);

    // Ожидаем: 0, 1, 0, 1, 2 (длина 5)
    EXPECT_EQ(concatenated->get_length(), 5);
    EXPECT_EQ(concatenated->get(1), 1); // Конец первого списка
    EXPECT_EQ(concatenated->get(2), 0); // Начало второго списка
    EXPECT_EQ(concatenated->get(4), 2); // Конец второго списка
}

TEST(LazyOperationsTest, MutationsAreImmutable) {
    LazyContext ctx;
    auto* gen = ctx.Allocate<FunctionGenerator<int>>(SequentialRule, Cardinal(3));
    auto* base_seq = ctx.Allocate<LazySequence<int>>(gen, &ctx); // [0, 1, 2]

    // 1. Append
    Sequence<int>* appended = base_seq->append(99);
    EXPECT_EQ(appended->get_length(), 4);
    EXPECT_EQ(appended->get_last(), 99);
    
    // Проверка иммутабельности: исходный список НЕ ИЗМЕНИЛСЯ
    EXPECT_EQ(base_seq->get_length(), 3); 

    // 2. Prepend
    Sequence<int>* prepended = base_seq->prepend(88);
    EXPECT_EQ(prepended->get_length(), 4);
    EXPECT_EQ(prepended->get_first(), 88);
    EXPECT_EQ(prepended->get(1), 0);

    // 3. Insert At (вставляем 77 на индекс 1)
    Sequence<int>* inserted = base_seq->insert_at(77, 1);
    // Ожидаем: [0, 77, 1, 2]
    EXPECT_EQ(inserted->get_length(), 4);
    EXPECT_EQ(inserted->get(0), 0);
    EXPECT_EQ(inserted->get(1), 77);
    EXPECT_EQ(inserted->get(2), 1);

    // 4. Remove At (удаляем элемент на индексе 1, это число '1')
    Sequence<int>* removed = base_seq->remove_at(1);
    // Ожидаем: [0, 2]
    EXPECT_EQ(removed->get_length(), 2);
    EXPECT_EQ(removed->get(0), 0);
    EXPECT_EQ(removed->get(1), 2);
}

TEST(LazyInfrastructureTest, BuilderCreatesLazySequence) {
    LazyContext ctx;
    
    // Создаем "пустышку", чтобы просто дернуть create_builder()
    auto* dummy_gen = ctx.Allocate<FunctionGenerator<int>>(SequentialRule, Cardinal(0));
    auto* dummy_seq = ctx.Allocate<LazySequence<int>>(dummy_gen, &ctx);

    // Берем билдер
    ISequenceBuilder<int>* builder = dummy_seq->create_builder();
    builder->append(100);
    builder->append(200);
    builder->append(300);

    // Собираем коллекцию
    Sequence<int>* result_seq = builder->build();

    // Проверяем, что создалась валидная ленивая коллекция
    EXPECT_EQ(result_seq->get_length(), 3);
    EXPECT_EQ(result_seq->get_first(), 100);
    EXPECT_EQ(result_seq->get_last(), 300);
}

TEST(LazyInfrastructureTest, EnumeratorIteratesCorrectly) {
    LazyContext ctx;
    auto* gen = ctx.Allocate<FunctionGenerator<int>>(SequentialRule, Cardinal(3));
    auto* seq = ctx.Allocate<LazySequence<int>>(gen, &ctx); // [0, 1, 2]

    IEnumerator<int>* enumerator = seq->get_enumerator();

    // Первый сдвиг (на индекс 0)
    EXPECT_TRUE(enumerator->move_next());
    EXPECT_EQ(enumerator->get_current(), 0);

    // Второй сдвиг (на индекс 1)
    EXPECT_TRUE(enumerator->move_next());
    EXPECT_EQ(enumerator->get_current(), 1);

    // Третий сдвиг (на индекс 2)
    EXPECT_TRUE(enumerator->move_next());
    EXPECT_EQ(enumerator->get_current(), 2);

    // Четвертый сдвиг (конец коллекции)
    EXPECT_FALSE(enumerator->move_next());

    // Проверка сброса
    enumerator->reset();
    EXPECT_TRUE(enumerator->move_next());
    EXPECT_EQ(enumerator->get_current(), 0);
}

// Функция для теста reduce
int SumReducer(const int& acc, const int& val) {
    return acc + val;
}

TEST(LazySequenceEdgeCases, ReduceWorksOnFiniteSequence) {
    LazyContext ctx;
    auto* gen = ctx.Allocate<FunctionGenerator<int>>(SequentialRule, Cardinal(4)); // 0, 1, 2, 3
    auto* seq = ctx.Allocate<LazySequence<int>>(gen, &ctx);

    // 0 + 1 + 2 + 3 = 6
    int sum = seq->reduce(SumReducer, 0);
    EXPECT_EQ(sum, 6);
}

TEST(LazySequenceEdgeCases, OutOfBoundsAccessThrows) {
    LazyContext ctx;
    auto* gen = ctx.Allocate<FunctionGenerator<int>>(SequentialRule, Cardinal(2)); // 0, 1
    auto* seq = ctx.Allocate<LazySequence<int>>(gen, &ctx);

    EXPECT_THROW(seq->get(-1), std::out_of_range); // Отрицательный индекс
    EXPECT_THROW(seq->get(5), std::out_of_range);  // Выход за пределы конечного списка
    
    auto* empty_seq = seq->create_empty();
    EXPECT_THROW(empty_seq->get_first(), std::out_of_range);
    EXPECT_THROW(empty_seq->get_last(), std::out_of_range);
}

// Правила для цепочки фильтров
bool IsEven(const int& v) { return v % 2 == 0; }
bool GreaterThanTen(const int& v) { return v > 10; }

TEST(LazySequenceEdgeCases, ChainedFiltersWork) {
    LazyContext ctx;
    auto* gen = ctx.Allocate<FunctionGenerator<int>>(SequentialRule); // 0, 1, 2, 3...
    auto* seq = ctx.Allocate<LazySequence<int>>(gen, &ctx);

    Sequence<int>* filtered = seq->where(IsEven)->where(GreaterThanTen);

    EXPECT_EQ(filtered->get(0), 12);
    EXPECT_EQ(filtered->get(1), 14);
    EXPECT_EQ(filtered->get(2), 16);
}   

TEST(LazyOperationsTest, InsertAfterMaterialized) {
    LazyContext ctx;
    auto* gen = ctx.Allocate<FunctionGenerator<int>>(SequentialRule); // 0, 1, 2, 3, 4...
    auto* seq = ctx.Allocate<LazySequence<int>>(gen, &ctx);

    // Читаем первые 3 элемента (индексы 0, 1, 2). Кэш = 3
    EXPECT_EQ(seq->get(2), 2); 
    EXPECT_EQ(seq->get_materialized_count(), 3);

    // Вставляем 99 сразу после вычисленных
    Sequence<int>* modified = seq->insert_after_materialized(99);

    // Ожидаем: 0, 1, 2, 99, 3, 4, 5...
    EXPECT_EQ(modified->get(0), 0);
    EXPECT_EQ(modified->get(2), 2);
    EXPECT_EQ(modified->get(3), 99); // Наш вставленный элемент
    EXPECT_EQ(modified->get(4), 3);  // Исходный список продолжился
}

TEST(LazyOperationsTest, InsertAndRestart) {
    LazyContext ctx;
    auto* gen = ctx.Allocate<FunctionGenerator<int>>(SequentialRule); // 0, 1, 2, 3, 4...
    auto* seq = ctx.Allocate<LazySequence<int>>(gen, &ctx);

    // Читаем первые 2 элемента (индексы 0, 1). Кэш = 2
    EXPECT_EQ(seq->get(1), 1); 

    // Вставляем 99 и начинаем заново
    Sequence<int>* modified = seq->insert_and_restart(99);

    // Ожидаем: [0, 1] + [99] + [0, 1, 2, 3...]
    EXPECT_EQ(modified->get(0), 0);
    EXPECT_EQ(modified->get(1), 1);
    EXPECT_EQ(modified->get(2), 99); // Вставленный элемент
    EXPECT_EQ(modified->get(3), 0);  // Список начался заново
    EXPECT_EQ(modified->get(4), 1);
}