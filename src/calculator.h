#pragma once

#include <string>
#include <string_view>

namespace calculator
{

    // Evaluates a mathematical expression.
    // Supported: +, -, *, /, parentheses, unary minus, integers/decimals, whitespace.
    // Throws std::runtime_error on invalid expressions.
    double evaluate(std::string_view expression);

    // Formats the resulting number for display.
    // - Prints integers without trailing .0 when close enough
    // - Otherwise prints with reasonable precision and trims trailing zeros
    std::string format_number(double value);

} // namespace calculator
