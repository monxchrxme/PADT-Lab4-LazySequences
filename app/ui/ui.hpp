#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "FsmRuleEngine.hpp"
#include "mutable_array_sequence.hpp" 

// Наши состояния и события
enum class State { START, SEEN_A, SUCCESS, FAIL };
enum class Event { CHAR_A, CHAR_B, CHAR_OTHER };

class AppUI {
private:
    sf::RenderWindow window;
    sf::Clock deltaClock;

    // Данные для ручного режима
    FsmRuleEngine<State, Event> rules;
    State current_state;

    MutableArraySequence<std::string> transition_log; 

    // Данные для стресс-теста
    int stress_test_amount = 1000000;
    float last_test_time_ms = 0.0f;
    std::string test_result = "Not run yet";

    float ui_scale = 1.0f;

    // Вспомогательные методы
    std::string StateToString(State s);
    void ProcessManualEvent(Event e);
    void RunStressTest();
    void ResetFSM();

public:
    AppUI();
    ~AppUI();
    void Run();
};