#include "calculator.h"

#include <cmath>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace std;

namespace
{
    // Implementation details for the expression evaluator.
    // Token kinds produced by the tokenizer and consumed by the parser/evaluator.
    enum class TokenKind
    {
        Number,
        Plus,
        Minus,
        Multiply,
        Divide,
        LParen,
        RParen,
        UnaryMinus,
    };

    // A single token in the expression.
    struct Token
    {
        TokenKind kind;
        double value = 0.0; // Used only when kind == TokenKind::Number.
    };

    // Returns true if the token kind is an operator (unary or binary).
    bool is_operator(TokenKind kind)
    {
        switch (kind)
        {
        case TokenKind::Plus:
        case TokenKind::Minus:
        case TokenKind::Multiply:
        case TokenKind::Divide:
        case TokenKind::UnaryMinus:
            return true;
        default:
            return false;
        }
    }

    // Operator precedence (higher binds tighter).
    int precedence(TokenKind kind)
    {
        switch (kind)
        {
        case TokenKind::UnaryMinus:
            return 3;
        case TokenKind::Multiply:
        case TokenKind::Divide:
            return 2;
        case TokenKind::Plus:
        case TokenKind::Minus:
            return 1;
        default:
            return 0;
        }
    }

    // Unary minus is right-associative (treats "--1" as -( -1 )).
    bool is_right_associative(TokenKind kind)
    {
        return kind == TokenKind::UnaryMinus;
    }

    // Helper to throw parse errors consistently.
    [[noreturn]] void parse_error(string msg)
    {
        throw runtime_error(std::move(msg));
    }

    // Convert raw input characters into a sequence of tokens.
    // Handles unary minus by emitting TokenKind::UnaryMinus where appropriate.
    vector<Token> tokenize(string_view input)
    {
        vector<Token> tokens;
        size_t i = 0;

        // Peek the previous emitted token kind (used to decide unary vs binary '-').
        auto peek_prev_kind = [&]() -> optional<TokenKind>
        {
            if (tokens.empty())
                return nullopt;
            return tokens.back().kind;
        };

        while (i < input.size())
        {
            char c = input[i];

            if (isspace(static_cast<unsigned char>(c)) != 0)
            {
                ++i;
                continue;
            }

            // Number: digits with an optional decimal point. Allows leading '.' (e.g. .5).
            if (isdigit(static_cast<unsigned char>(c)) != 0 || c == '.')
            {
                size_t start = i;
                bool seen_dot = (c == '.');
                ++i;
                while (i < input.size())
                {
                    char d = input[i];
                    if (isdigit(static_cast<unsigned char>(d)) != 0)
                    {
                        ++i;
                        continue;
                    }
                    if (d == '.' && !seen_dot)
                    {
                        seen_dot = true;
                        ++i;
                        continue;
                    }
                    break;
                }

                string number_str(input.substr(start, i - start));
                char *endptr = nullptr;
                double value = strtod(number_str.c_str(), &endptr);
                if (endptr == nullptr || *endptr != '\0')
                {
                    parse_error("Invalid number: " + number_str);
                }
                tokens.push_back(Token{TokenKind::Number, value});
                continue;
            }

            switch (c)
            {
            case '+':
                tokens.push_back(Token{TokenKind::Plus});
                ++i;
                break;
            case '-':
            {
                // Determine unary vs binary based on previous token.
                // Unary when expression starts or follows '(' or another operator.
                auto prev = peek_prev_kind();
                bool unary = !prev.has_value() || *prev == TokenKind::LParen || is_operator(*prev);
                tokens.push_back(Token{unary ? TokenKind::UnaryMinus : TokenKind::Minus});
                ++i;
                break;
            }
            case '*':
                tokens.push_back(Token{TokenKind::Multiply});
                ++i;
                break;
            case '/':
                tokens.push_back(Token{TokenKind::Divide});
                ++i;
                break;
            case '(':
                tokens.push_back(Token{TokenKind::LParen});
                ++i;
                break;
            case ')':
                tokens.push_back(Token{TokenKind::RParen});
                ++i;
                break;
            default:
                parse_error(string("Unexpected character: '") + c + "'");
            }
        }

        return tokens;
    }

    // Convert tokens from infix form to Reverse Polish Notation (postfix).
    // Uses the shunting-yard algorithm to enforce precedence and parentheses.
    vector<Token> to_rpn(const vector<Token> &tokens)
    {
        vector<Token> output;
        vector<Token> op_stack;

        for (const auto &tok : tokens)
        {
            switch (tok.kind)
            {
            case TokenKind::Number:
                output.push_back(tok);
                break;
            case TokenKind::LParen:
                op_stack.push_back(tok);
                break;
            case TokenKind::RParen:
            {
                bool found_lparen = false;
                while (!op_stack.empty())
                {
                    Token top = op_stack.back();
                    op_stack.pop_back();
                    if (top.kind == TokenKind::LParen)
                    {
                        found_lparen = true;
                        break;
                    }
                    output.push_back(top);
                }
                if (!found_lparen)
                {
                    parse_error("Mismatched parentheses");
                }
                break;
            }
            case TokenKind::Plus:
            case TokenKind::Minus:
            case TokenKind::Multiply:
            case TokenKind::Divide:
            case TokenKind::UnaryMinus:
            {
                while (!op_stack.empty())
                {
                    TokenKind top_kind = op_stack.back().kind;
                    if (!is_operator(top_kind))
                    {
                        break;
                    }

                    int p1 = precedence(tok.kind);
                    int p2 = precedence(top_kind);

                    bool should_pop = is_right_associative(tok.kind) ? (p1 < p2) : (p1 <= p2);
                    if (!should_pop)
                    {
                        break;
                    }

                    output.push_back(op_stack.back());
                    op_stack.pop_back();
                }
                op_stack.push_back(tok);
                break;
            }
            default:
                parse_error("Unexpected token during parsing");
            }
        }

        while (!op_stack.empty())
        {
            if (op_stack.back().kind == TokenKind::LParen || op_stack.back().kind == TokenKind::RParen)
            {
                parse_error("Mismatched parentheses");
            }
            output.push_back(op_stack.back());
            op_stack.pop_back();
        }

        return output;
    }

    // Evaluate an RPN (postfix) token stream using a simple value stack.
    double eval_rpn(const vector<Token> &rpn)
    {
        vector<double> st;

        auto pop = [&]() -> double
        {
            if (st.empty())
            {
                parse_error("Invalid expression");
            }
            double v = st.back();
            st.pop_back();
            return v;
        };

        for (const auto &tok : rpn)
        {
            switch (tok.kind)
            {
            case TokenKind::Number:
                st.push_back(tok.value);
                break;
            case TokenKind::UnaryMinus:
            {
                double a = pop();
                st.push_back(-a);
                break;
            }
            case TokenKind::Plus:
            {
                double b = pop();
                double a = pop();
                st.push_back(a + b);
                break;
            }
            case TokenKind::Minus:
            {
                double b = pop();
                double a = pop();
                st.push_back(a - b);
                break;
            }
            case TokenKind::Multiply:
            {
                double b = pop();
                double a = pop();
                st.push_back(a * b);
                break;
            }
            case TokenKind::Divide:
            {
                double b = pop();
                double a = pop();
                if (b == 0.0)
                {
                    parse_error("Division by zero");
                }
                st.push_back(a / b);
                break;
            }
            default:
                parse_error("Unexpected token during evaluation");
            }
        }

        if (st.size() != 1)
        {
            parse_error("Invalid expression");
        }

        return st.back();
    }

} // namespace

namespace calculator
{
    // Public entrypoint: tokenize -> parse (RPN) -> evaluate.
    double evaluate(string_view expression)
    {
        auto tokens = tokenize(expression);
        auto rpn = to_rpn(tokens);
        return eval_rpn(rpn);
    }

    // Format numbers for CLI output (avoid trailing zeros, show integers cleanly).
    string format_number(double value)
    {
        if (isnan(value))
            return "nan";
        if (isinf(value))
            return (value > 0 ? "inf" : "-inf");

        // Print integers without trailing .0 when the result is very close.
        double rounded = round(value);
        if (fabs(value - rounded) < 1e-12 &&
            rounded >= static_cast<double>(numeric_limits<long long>::min()) &&
            rounded <= static_cast<double>(numeric_limits<long long>::max()))
        {
            return to_string(static_cast<long long>(rounded));
        }

        ostringstream oss;
        oss << setprecision(15) << value;
        string s = oss.str();

        // Trim trailing zeros in decimals for nicer output.
        auto dot_pos = s.find('.');
        if (dot_pos != string::npos)
        {
            while (!s.empty() && s.back() == '0')
                s.pop_back();
            if (!s.empty() && s.back() == '.')
                s.pop_back();
        }

        return s;
    }

} // namespace calculator
