cmake_minimum_required(VERSION 2.6)
project(Automaton)
set(EXECUTABLE_OUTPUT_PATH ${Automaton_SOURCE_DIR}/../bin)
set(LIBRARY_OUTPUT_PATH ${Automaton_SOURCE_DIR}/../bin)
set(Automaton_INCLUDE_DIR ${Automaton_SOURCE_DIR}/../include)

if (WIN32 AND NOT UNIX)
add_definitions(/DWINDOWS)
endif()

if(UNIX)
add_definitions(-DUNIX)
add_definitions(-g)
endif()

add_subdirectory(../src ../build/temp)
