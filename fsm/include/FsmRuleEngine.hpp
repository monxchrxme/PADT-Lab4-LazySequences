#pragma once
#include "mutable_array_sequence.hpp"

// Класс хранит правила перехода: (Текущее состояние + Событие) -> Новое состояние
template <typename TState, typename TEvent>
class FsmRuleEngine {
private:
    struct Transition {
        TState current_state;
        TEvent event;
        TState next_state;
    };

    MutableArraySequence<Transition> transitions;
    TState fallback_state; // Состояние по умолчанию, если правило не найдено

public:
    explicit FsmRuleEngine(TState fallback) : fallback_state(fallback) {}

    void add_rule(const TState& current, const TEvent& event, const TState& next) {
        Transition t = {current, event, next};
        transitions.append(t); 
    }

    // Получить следующее состояние на основе текущего и события
    TState get_next_state(const TState& current, const TEvent& event) const {
        for (int i = 0; i < transitions.get_length(); ++i) {
            const Transition& t = transitions.get(i);
            // Если состояние и событие совпали - переходим
            // Предполагается, что для TState и TEvent перегружен оператор ==
            if (t.current_state == current && t.event == event) {
                return t.next_state;
            }
        }
        return fallback_state; // Если правило не описано, переходим в fallback
    }
};