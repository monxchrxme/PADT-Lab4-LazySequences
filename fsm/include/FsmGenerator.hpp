#pragma once
#include "Generator.hpp"
#include "StreamInterfaces.hpp"
#include "FsmRuleEngine.hpp"

template <typename TState, typename TEvent>
class FsmGenerator : public Generator<TState> {
private:
    IReadOnlyStream<TEvent>* input_stream;
    const FsmRuleEngine<TState, TEvent>* rule_engine;
    TState current_state;

public:
    // Автомат принимает поток событий, набор правил и начальное состояние
    FsmGenerator(IReadOnlyStream<TEvent>* stream, 
                 const FsmRuleEngine<TState, TEvent>* rules, 
                 const TState& initial_state)
        : input_stream(stream), rule_engine(rules), current_state(initial_state) {}

    Option<TState> TryGetNext() override {
        if (input_stream->is_end_of_stream()) {
            return Option<TState>();
        }
        // Читаем одно событие из потока 
        TEvent event = input_stream->read();
        // Меняем состояние по правилам
        current_state = rule_engine->get_next_state(current_state, event);
        // Возвращаем новое состояние
        return Option<TState>(current_state);
    }

    Cardinal get_cardinality() const override {
        // Длина потока состояний равна длине потока событий
        // Так как мы не всегда можем знать длину входного потока, 
        // безопасно вернуть Infinity (перекидываем ответственность на ленивый список)
        return Cardinal::Infinity(); 
    }
};