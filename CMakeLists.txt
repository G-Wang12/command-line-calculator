cmake_minimum_required(VERSION 3.16)

project(command_line_calculator LANGUAGES CXX)

add_executable(toy-app
    src/main.cpp
    src/calculator.cpp
)

target_compile_features(toy-app PRIVATE cxx_std_17)

# Reasonable warnings for local development.
if (MSVC)
    target_compile_options(toy-app PRIVATE /W4)
else()
    target_compile_options(toy-app PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()

add_executable(calculator-tests
    tests/test_calculator.cpp
    src/calculator.cpp
)

target_compile_features(calculator-tests PRIVATE cxx_std_17)

if (MSVC)
    target_compile_options(calculator-tests PRIVATE /W4)
else()
    target_compile_options(calculator-tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_include_directories(calculator-tests PRIVATE src)

add_test(NAME calculator-tests COMMAND calculator-tests)
