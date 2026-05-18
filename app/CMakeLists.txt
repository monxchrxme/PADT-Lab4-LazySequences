add_executable(Lab4_app)

target_sources(Lab4_app PRIVATE
    src/main.cpp
    ui/ui.cpp
)

target_include_directories(Lab4_app PRIVATE ui)
target_link_libraries(Lab4_app PRIVATE 
    fsm_lib 
    ImGui-SFML::ImGui-SFML
)