cmake_minimum_required(VERSION 3.16)
project(TextFreqAnalyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(textfreq_lib
    src/json_parser.cpp
    src/text_analyzer.cpp
    src/cli.cpp
)

target_include_directories(textfreq_lib PUBLIC include)

add_executable(textfreq_cli src/main.cpp)
target_link_libraries(textfreq_cli PRIVATE textfreq_lib)

add_executable(textfreq_tests tests/tests_main.cpp)
target_link_libraries(textfreq_tests PRIVATE textfreq_lib)


