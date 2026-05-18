#include "ui.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <chrono>

#include "LazyContext.hpp"
#include "FunctionGenerator.hpp"
#include "GeneratorReadStream.hpp"
#include "FsmGenerator.hpp"

// Настраиваем правила автомата при запуске UI
AppUI::AppUI() : window(sf::VideoMode(1920, 1080), "Lab 4: Lazy FSM"), 
                 rules(State::START), 
                 current_state(State::START) {
    ImGui::SFML::Init(window);

    // Паттерн "AB"
    rules.add_rule(State::START, Event::CHAR_A, State::SEEN_A);
    // Если мы увидели A, а потом снова A - мы всё еще ждем B
    rules.add_rule(State::SEEN_A, Event::CHAR_A, State::SEEN_A); 
    // Если увидели B после A - Успех
    rules.add_rule(State::SEEN_A, Event::CHAR_B, State::SUCCESS);
    // Если мы в SUCCESS и пришло A - начинаем новый поиск
    rules.add_rule(State::SUCCESS, Event::CHAR_A, State::SEEN_A);

    // ВСЕ ОСТАЛЬНЫЕ переходы (мусор) автоматически приведут к сбросу в START 
    // благодаря fallback-состоянию в конструкторе rules(State::START)
    
    transition_log.append("System initialized. State: START");
}

void AppUI::ResetFSM() {
    current_state = State::START;
    transition_log.append("FSM MANUALLY RESET");
}

AppUI::~AppUI() {
    ImGui::SFML::Shutdown();
}

std::string AppUI::StateToString(State s) {
    switch (s) {
        case State::START: return "START";
        case State::SEEN_A: return "SEEN_A";
        case State::SUCCESS: return "SUCCESS";
        case State::FAIL: return "FAIL";
        default: return "UNKNOWN";
    }
}

void AppUI::ProcessManualEvent(Event e) {
    State next = rules.get_next_state(current_state, e);
    std::string e_str = (e == Event::CHAR_A) ? "'A'" : (e == Event::CHAR_B) ? "'B'" : "'*' (Other)";
    
    transition_log.append("Event: " + e_str + " | Transition: " + 
                          StateToString(current_state) + " -> " + StateToString(next));
    
    current_state = next;
    
    // Ограничиваем лог (удаляем старые записи, чтобы не рос бесконечно)
    if (transition_log.get_length() > 15) {
        transition_log.remove_at(0); 
    }
}

static int g_target_length = 0;
Option<Event> UIMassiveDataRule(int index) {
    if (index < g_target_length - 2) return Option<Event>(Event::CHAR_OTHER);
    if (index == g_target_length - 2) return Option<Event>(Event::CHAR_A);
    if (index == g_target_length - 1) return Option<Event>(Event::CHAR_B);
    return Option<Event>(); 
}

void AppUI::RunStressTest() {
    g_target_length = stress_test_amount;
    LazyContext ctx;

    auto* gen = ctx.Allocate<FunctionGenerator<Event>>(UIMassiveDataRule, Cardinal(g_target_length));
    GeneratorReadStream<Event> input_stream(gen);
    input_stream.open();

    auto* fsm_gen = ctx.Allocate<FsmGenerator<State, Event>>(&input_stream, &rules, State::START);
    
    State final_state = State::START;
    int processed = 0;

    auto start_time = std::chrono::high_resolution_clock::now();

    while (true) {
        Option<State> next = fsm_gen->TryGetNext();
        if (!next.is_some()) break;
        final_state = next.unwrap();
        processed++;

        // АНТИ-ЗАВИСАНИЕ (UI PUMP)
        // Каждые 1 000 000 элементов "даем окну подышать", чтобы Windows не думал, что мы зависли
        if (processed % 1000000 == 0) {
            sf::Event event;
            while (window.pollEvent(event)) {
                // Просто читаем события ОС и откидываем их в пустоту
                // (или закрываем окно, если пользователь нажал крестик во время теста)
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return; // Экстренное прерывание
                }
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    last_test_time_ms = std::chrono::duration<float, std::milli>(end_time - start_time).count();
    
    input_stream.close();

    test_result = "Processed: " + std::to_string(processed) + 
                  " elements. Final State: " + StateToString(final_state);
}

void AppUI::Run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed) window.close();
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::GetIO().FontGlobalScale = ui_scale;

        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("FSM Control Panel", nullptr, ImGuiWindowFlags_NoCollapse);

        ImGui::SliderFloat("UI Scale", &ui_scale, 0.5f, 3.0f, "%.1f");
        ImGui::Separator();

        if (ImGui::BeginTabBar("Tabs")) {
            
            // ВКЛАДКА 1: РУЧНОЙ РЕЖИМ
            if (ImGui::BeginTabItem("Manual Mode")) {
                ImGui::Text("Current State: ");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", StateToString(current_state).c_str());
                
                ImGui::SameLine(0, 50 * ui_scale); 
                if (ImGui::Button("Reset FSM")) ResetFSM();

                ImGui::SameLine(0, 20 * ui_scale);
                if (ImGui::Button("Copy Log to Clipboard")) {
                    std::string clipboard_text;
                    for (int i = 0; i < transition_log.get_length(); ++i) {
                        clipboard_text += transition_log.get(i) + "\n";
                    }
                    ImGui::SetClipboardText(clipboard_text.c_str()); 
                }

                ImGui::Separator();
                ImGui::Text("Inject Event:");
                if (ImGui::Button("Send 'A'", ImVec2(100 * ui_scale, 0))) ProcessManualEvent(Event::CHAR_A);
                ImGui::SameLine();
                if (ImGui::Button("Send 'B'", ImVec2(100 * ui_scale, 0))) ProcessManualEvent(Event::CHAR_B);
                ImGui::SameLine();
                if (ImGui::Button("Send Other", ImVec2(120 * ui_scale, 0))) ProcessManualEvent(Event::CHAR_OTHER);

                ImGui::Separator();
                ImGui::Text("Transition Log:");
                
                ImGui::BeginChild("LogRegion", ImVec2(0, 0), true);
                for (int i = 0; i < transition_log.get_length(); ++i) {
                    ImGui::TextUnformatted(transition_log.get(i).c_str());
                }
                // Автоскролл только если мы были в самом низу
                if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                    ImGui::SetScrollHereY(1.0f);
                ImGui::EndChild();

                ImGui::EndTabItem();
            }

            // ВКЛАДКА 2: СТРЕСС ТЕСТ
            if (ImGui::BeginTabItem("Stress Test (Lazy streams)")) {
                ImGui::Text("Test generating millions of elements with O(1) memory footprint.");
                ImGui::Separator();
                
                ImGui::SliderInt("Elements Count", &stress_test_amount, 10000, 100000000, "%d elements");
                
                if (ImGui::Button("Set 1 Million")) stress_test_amount = 1000000;
                ImGui::SameLine();
                if (ImGui::Button("Set 10 Million")) stress_test_amount = 10000000;
                ImGui::SameLine();
                if (ImGui::Button("Set 25 Million")) stress_test_amount = 25000000;
                ImGui::SameLine();
                if (ImGui::Button("Set 50 Million")) stress_test_amount = 50000000;
                ImGui::SameLine();
                if (ImGui::Button("Set 100 Million")) stress_test_amount = 100000000;

                ImGui::Separator();
                if (ImGui::Button("Run Benchmark", ImVec2(200 * ui_scale, 0))) {
                    RunStressTest();
                }

                ImGui::Separator();
                ImGui::Text("Result: %s", test_result.c_str());
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "Time taken: %.2f ms", last_test_time_ms);

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::End();

        window.clear(sf::Color(40, 40, 40));
        ImGui::SFML::Render(window);
        window.display();
    }
}