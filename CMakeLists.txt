cmake_minimum_required(VERSION 3.26)
project(chess-analysis C)
include(CTest)

set(CMAKE_C_STANDARD 11)

add_library(chess OBJECT board.c board.h parser.c parser.h panic.c panic.h tools.c tools.h parser_helpers.c parser_helpers.h)

add_executable(chess-analysis main.c)
add_executable(tests tests.c)

target_link_libraries(chess-analysis chess)
target_link_libraries(tests chess)

add_test(NAME chess_tests COMMAND tests)