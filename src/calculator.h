#pragma once

#include <string>
#include <string_view>

namespace calculator
{

    /**
     * Evaluate a mathematical expression.
     *
     * Supported features:
     * - Binary operators: +, -, *, /
     * - Parentheses: ( )
     * - Unary minus (negation): -3, -(2+1)
     * - Integers and decimals (including leading '.', e.g. .5)
     * - Arbitrary whitespace
     *
     * @param expression The expression to evaluate. This is a non-owning view;
     *        the referenced characters must remain valid for the duration of
     *        the call.
     * @return The computed value as a double.
     * @throws std::runtime_error If the expression is invalid (syntax error,
     *         mismatched parentheses, division by zero, etc.).
     */
    double evaluate(std::string_view expression);

    /**
     * Format a number for CLI display.
     *
     * Behavior:
     * - If the value is very close to an integer, prints it without trailing ".0".
     * - Otherwise prints with a reasonable precision and trims trailing zeros.
     * - Prints "nan" / "inf" / "-inf" for special values.
     */
    std::string format_number(double value);

} // namespace calculator
