file(GLOB files "*.cpp")
include_directories(${Automaton_INCLUDE_DIR}/client)
add_executable(automaton ${files})
target_link_libraries(automaton lua)
