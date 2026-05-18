#include <gtest/gtest.h>
#include "LazyContext.hpp"
#include "FsmRuleEngine.hpp"
#include "FsmGenerator.hpp"
#include "FunctionGenerator.hpp"
#include "GeneratorReadStream.hpp"

// Перечисления для состояний и событий
enum class State { START, SEEN_A, SUCCESS, FAIL };
enum class Event { CHAR_A, CHAR_B, CHAR_OTHER };

int test_data_length = 10000000; // по умолчанию 10 млн  

// Правило для тестовых данных: Выдаем (test_data_length - 2) раз CHAR_OTHER, затем CHAR_A, затем CHAR_B
Option<Event> MassiveDataRule(int index) {
    if (index < test_data_length - 2) return Option<Event>(Event::CHAR_OTHER);
    if (index == test_data_length - 2) return Option<Event>(Event::CHAR_A);
    if (index == test_data_length - 1) return Option<Event>(Event::CHAR_B);
    return Option<Event>(); // Конец потока (test_data_length элементов)
}

TEST(FsmTest, StateMachineLogicAndStressTest) {
    LazyContext ctx;

    // 1. Настраиваем правила автомата
    FsmRuleEngine<State, Event> rules(State::FAIL); // По умолчанию - FAIL
    
    // Ищем паттерн "A" -> "B"
    rules.add_rule(State::START, Event::CHAR_OTHER, State::START); // Игнорируем мусор
    rules.add_rule(State::START, Event::CHAR_A, State::SEEN_A);    // Увидели A
    rules.add_rule(State::SEEN_A, Event::CHAR_B, State::SUCCESS);  // Увидели B после A - Успех
    rules.add_rule(State::SEEN_A, Event::CHAR_OTHER, State::START); // Ошибка, сброс

    // 2. Создаем "гигантский" источник данных (test_data_length событий)
    auto* huge_gen = ctx.Allocate<FunctionGenerator<Event>>(MassiveDataRule, Cardinal(test_data_length));
    GeneratorReadStream<Event> input_stream(huge_gen);
    input_stream.open();

    // 3. Создаем Автомат Состояний
    auto* fsm_gen = ctx.Allocate<FsmGenerator<State, Event>>(&input_stream, &rules, State::START);
    
    // 4. Оборачиваем Автомат в ленивую коллекцию (будем читать напрямую из генератора)
    
    State final_state = State::START;
    int processed_count = 0;

    while (true) {
        Option<State> next_state = fsm_gen->TryGetNext();
        if (!next_state.is_some()) break; // Поток иссяк
        
        final_state = next_state.unwrap();
        processed_count++;
    }

    input_stream.close();

    EXPECT_EQ(processed_count, test_data_length); // Обработали test_data_length элементов
    EXPECT_EQ(final_state, State::SUCCESS); // Паттерн "AB" в самом конце был найден
}