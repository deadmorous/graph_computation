/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "common/expr_calculator.hpp"

#include "common/throw.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace common {

/* NOTE: Initially, the implementation was fully vibe-coded using Google Gemini
 *
 * Here is the prompt:
 *
 * Hi, I need an expression calculator functionality in C++. The expressions
 * would have a traditional form and depend on variables, e.g.
 *
 * (((n+4)/8+127)%256)-127
 * or
 * 127*sin(1.25*3.14159*n/127/8)
 *
 * where n is a variable that should be bound to a numeric value before
 * calculation.
 *
 * It's fine to just do floating-point calculations, with values of type double;
 * the modulo operation, and also bitwise operations, require rounding off
 * operands before application.
 *
 * I need a class with the following interface:
 *
 * class ExprCalculator
 * {
 * public:
 *   explicit ExprCalculator(std::string_view expr);
 *   auto operator()(
 *      const std::unordered_map<std::string_view, double>& variables) const
 *          -> double;
 * };
 *
 * The implementation is not performance-critical; nevertheless, I would expect
 * the constructor to compile the expressions into instructions for a stack
 * machine, which are run when operator() is invoked.
 *
 * Can you please help with that?
 * ---
 *
 * Later a few changes were made manually:
 * - Attempt to parse a number no longer relies on exceptions
 * - Bug fixed: ensure that invalid expressions cause compile errors, rather
 *   than failures at evaluation time. For example. '1+' passed compilation
 *   but caused a crash at evaluation time
 * - Tokenizer does not start with removing all whitespace (that might lead
 *   to gluing numbers in an invalid expressions). Whitespaces are ignored
 *   between tokens.
 */

/**
 * @brief Class to parse, compile, and execute mathematical expressions.
 * The compilation phase converts the infix expression to RPN (Reverse Polish Notation)
 * and stores it as a sequence of 'Instructions' for a stack machine.
 */
class ExprCalculator::Impl final
{
    // --- Instruction Set (The Stack Machine "Bytecode") ---
    enum class OpCode {
        PUSH_LITERAL, // Push a constant double value
        PUSH_VARIABLE, // Push a variable's value (lookup by name)
        ADD, SUB, MUL, DIV, // Basic arithmetic
        MOD, // Modulo (requires rounding)
        POW, // Power function
        NEGATE, // Unary minus
        // Functions (e.g., sin, cos, etc.)
        FN_SIN, FN_COS, FN_TAN,
        FN_ASIN, FN_ACOS, FN_ATAN,
        FN_LOG, FN_LOG10, FN_EXP,
        FN_SQRT, FN_ABS,
        // Bitwise operations (require rounding/casting)
        BIT_AND, BIT_OR, BIT_XOR, BIT_NOT
    };

    struct Instruction {
        OpCode code;
        double literal_value = 0.0; // Used by PUSH_LITERAL
        std::string variable_name;   // Used by PUSH_VARIABLE
    };

    std::vector<Instruction> compiled_instructions;

    // --- Utility Functions for Parsing and Execution ---

    // Operator precedence: higher number means higher precedence
    int get_precedence(std::string_view op) const {
        if (op == "+" || op == "-") return 1;
        if (op == "*" || op == "/" || op == "%") return 2;
        if (op == "^") return 3; // Power (right-associative)
        if (op == "~") return 4; // Unary negation (custom symbol for internal use)
        return 0;
    }

    // Check if an operator is left-associative
    bool is_left_associative(std::string_view op) const {
        return op == "+" || op == "-" || op == "*" || op == "/" || op == "%";
    }

    // Helper to map string token to instruction
    Instruction token_to_instruction(std::string_view token) {
        if (token == "+") return {OpCode::ADD};
        if (token == "-") return {OpCode::SUB};
        if (token == "*") return {OpCode::MUL};
        if (token == "/") return {OpCode::DIV};
        if (token == "%") return {OpCode::MOD};
        if (token == "^") return {OpCode::POW};
        if (token == "~") return {OpCode::NEGATE}; // Unary minus

        if (token == "sin") return {OpCode::FN_SIN};
        if (token == "cos") return {OpCode::FN_COS};
        if (token == "tan") return {OpCode::FN_TAN};
        if (token == "asin") return {OpCode::FN_ASIN};
        if (token == "acos") return {OpCode::FN_ACOS};
        if (token == "atan") return {OpCode::FN_ATAN};
        if (token == "log") return {OpCode::FN_LOG};
        if (token == "log10") return {OpCode::FN_LOG10};
        if (token == "exp") return {OpCode::FN_EXP};
        if (token == "sqrt") return {OpCode::FN_SQRT};
        if (token == "abs") return {OpCode::FN_ABS};

        if (token == "&") return {OpCode::BIT_AND};
        if (token == "|") return {OpCode::BIT_OR};
        if (token == "^^") return {OpCode::BIT_XOR}; // Use ^^ for bitwise XOR
        if (token == "!") return {OpCode::BIT_NOT};

        // Should not be reached for operators/functions
        throw std::runtime_error("Unknown operator or function token: " + std::string(token));
    }

    int value_stack_delta(OpCode op_code) const
    {
        switch(op_code)
        {
        case OpCode::PUSH_LITERAL: return 1;
        case OpCode::PUSH_VARIABLE: return 1;
        case OpCode::ADD: return -1;
        case OpCode::SUB: return -1;
        case OpCode::MUL: return -1;
        case OpCode::DIV: return -1;
        case OpCode::MOD: return -1;
        case OpCode::POW: return -1;
        case OpCode::NEGATE: return 0;
        case OpCode::FN_SIN: return 0;
        case OpCode::FN_COS: return 0;
        case OpCode::FN_TAN: return 0;
        case OpCode::FN_ASIN: return 0;
        case OpCode::FN_ACOS: return 0;
        case OpCode::FN_ATAN: return 0;
        case OpCode::FN_LOG: return 0;
        case OpCode::FN_LOG10: return 0;
        case OpCode::FN_EXP: return 0;
        case OpCode::FN_SQRT: return 0;
        case OpCode::FN_ABS: return 0;
        case OpCode::BIT_AND: return -1;
        case OpCode::BIT_OR: return -1;
        case OpCode::BIT_XOR: return -1;
        case OpCode::BIT_NOT: return 0;
        }
        __builtin_unreachable();
    }

    // Simple tokenizer for the expression string
    std::vector<std::string> tokenize(std::string_view s) {
        std::vector<std::string> tokens;
        for (size_t i = 0; i < s.length(); ++i) {
            char c = s[i];
            if (::isspace(c))
                continue;

            // Handle literals (numbers)
            if (isdigit(c) || c == '.') {
                std::string num;
                while (i < s.length() && (isdigit(s[i]) || s[i] == '.')) {
                    num += s[i++];
                }
                tokens.push_back(num);
                i--; // Backtrack one character
                continue;
            }

            // Handle function names and variable names
            if (isalpha(c) || c == '_') {
                std::string ident;
                while (i < s.length() && (isalnum(s[i]) || s[i] == '_')) {
                    ident += s[i++];
                }
                tokens.push_back(ident);
                i--; // Backtrack one character
                continue;
            }

            // Handle operators and parentheses
            std::string op;
            if (c == '+' || c == '*' || c == '/' || c == '%' || c == '(' || c == ')' || c == '^' || c == '&' || c == '|' || c == '!') {
                op += c;
            } else if (c == '-') {
                // Check for unary minus: it's unary if it follows '(', an operator, or is at the start
                bool is_unary = (i == 0) || (i > 0 && (s[i-1] == '(' || s[i-1] == '+' || s[i-1] == '-' || s[i-1] == '*' || s[i-1] == '/' || s[i-1] == '%' || s[i-1] == '^'));
                if (is_unary) {
                    op += '~'; // Internal token for UNARY MINUS
                } else {
                    op += '-'; // Binary subtraction
                }
            } else if (c == '^' && i + 1 < s.length() && s[i+1] == '^') {
                op = "^^"; // Bitwise XOR
                i++;
            } else {
                // Unknown character
                throw std::runtime_error("Invalid character in expression: " + std::string(1, c));
            }

            if (!op.empty()) {
                tokens.push_back(op);
            }
        }
        return tokens;
    }

public:
    /**
     * @brief Constructor: Parses and compiles the expression into RPN instructions.
     * Uses the Shunting-Yard algorithm.
     * @param expr The mathematical expression string.
     */
    explicit Impl(std::string_view expr)
    {
        std::vector<std::string> tokens = tokenize(expr);
        std::stack<std::string> op_stack;
        auto value_stack_pos = int{};
        auto push_instruction = [&](Instruction instruction)
        {
            value_stack_pos += value_stack_delta(instruction.code);
            if (value_stack_pos <= 0)
                throw std::runtime_error("Expression exhausts the value stack");
            compiled_instructions.push_back(instruction);
        };

        for (const auto& token : tokens) {
            // 1. Check if token is a number
            {
                auto val = double{};
                const auto* first = token.data();
                const auto* last = first + token.size();
                auto fc_result = std::from_chars(first, last, val);
                if (fc_result.ec == std::errc{})
                {
                    if (fc_result.ptr != last)
                        throw std::invalid_argument(
                            "Unexpected characters following numeric literal: '"
                            + token + "'");
                    push_instruction({OpCode::PUSH_LITERAL, val, ""});
                    continue;
                }
            }
            // Not a number, continue to check variables/operators

            // 2. Check if token is a variable or function name
            if (isalpha(token[0])) {
                // Treat as function if followed by '(', otherwise as variable
                if (token == "sin" || token == "cos" || token == "tan" || token == "asin" || token == "acos" ||
                    token == "atan" || token == "log" || token == "log10" || token == "exp" || token == "sqrt" || token == "abs") {
                    op_stack.push(token); // Push function onto the stack
                } else {
                    // Variable
                    push_instruction({OpCode::PUSH_VARIABLE, 0.0, token});
                }
                continue;
            }

            // 3. Handle parentheses
            if (token == "(") {
                op_stack.push(token);
                continue;
            }
            if (token == ")") {
                while (!op_stack.empty() && op_stack.top() != "(") {
                    push_instruction(token_to_instruction(op_stack.top()));
                    op_stack.pop();
                }
                if (op_stack.empty() || op_stack.top() != "(") {
                    throw std::runtime_error("Mismatched parentheses or missing operator.");
                }
                op_stack.pop(); // Pop '('

                // Check if a function is at the top of the stack (e.g., sin, cos)
                if (!op_stack.empty() && isalpha(op_stack.top()[0])) {
                    push_instruction(token_to_instruction(op_stack.top()));
                    op_stack.pop();
                }
                continue;
            }

            // 4. Handle operators (+, -, *, /, %, ^, ~, &, |, ^^, !)
            if (get_precedence(token) > 0) {
                while (!op_stack.empty() && op_stack.top() != "(") {
                    int p1 = get_precedence(token);
                    int p2 = get_precedence(op_stack.top());

                    if (p1 < p2 || (p1 == p2 && is_left_associative(token))) {
                        push_instruction(token_to_instruction(op_stack.top()));
                        op_stack.pop();
                    } else {
                        break;
                    }
                }
                op_stack.push(token);
                continue;
            }

            // Fallback for an unknown token that wasn't a number, variable, or operator
            throw std::runtime_error("Unrecognized token in expression: " + token);
        }

        // Pop all remaining operators from the stack to the output
        while (!op_stack.empty()) {
            if (op_stack.top() == "(") {
                throw std::runtime_error("Mismatched parentheses in expression.");
            }
            push_instruction(token_to_instruction(op_stack.top()));
            op_stack.pop();
        }

        if (value_stack_pos != 1)
            throw_<std::runtime_error>("Expression leaves ", value_stack_pos,
                                       " elements on the value stack");

    }

    /**
     * @brief Executes the compiled RPN instructions (the stack machine) with provided variables.
     * @param variables Map of variable names (string_view) to their double values.
     * @return The result of the expression evaluation as a double.
     */
    auto operator()(const VariableMap& variables) const -> double
    {
        std::stack<double> value_stack;

        for (const auto& instruction : compiled_instructions) {
            double v1, v2;

            switch (instruction.code) {
            case OpCode::PUSH_LITERAL:
                value_stack.push(instruction.literal_value);
                break;

            case OpCode::PUSH_VARIABLE:
                if (auto it = variables.find(instruction.variable_name); it != variables.end()) {
                    value_stack.push(it->second);
                } else {
                    throw std::runtime_error("Variable not bound: " + instruction.variable_name);
                }
                break;

            // Binary Operators
            case OpCode::ADD: v2 = value_stack.top(); value_stack.pop(); v1 = value_stack.top(); value_stack.pop(); value_stack.push(v1 + v2); break;
            case OpCode::SUB: v2 = value_stack.top(); value_stack.pop(); v1 = value_stack.top(); value_stack.pop(); value_stack.push(v1 - v2); break;
            case OpCode::MUL: v2 = value_stack.top(); value_stack.pop(); v1 = value_stack.top(); value_stack.pop(); value_stack.push(v1 * v2); break;
            case OpCode::DIV: v2 = value_stack.top(); value_stack.pop(); v1 = value_stack.top(); value_stack.pop(); value_stack.push(v1 / v2); break;
            case OpCode::POW: v2 = value_stack.top(); value_stack.pop(); v1 = value_stack.top(); value_stack.pop(); value_stack.push(std::pow(v1, v2)); break;

            case OpCode::MOD: { // Modulo (requires rounding)
                v2 = value_stack.top(); value_stack.pop();
                v1 = value_stack.top(); value_stack.pop();
                // Round operands before modulo
                long long i1 = static_cast<long long>(std::round(v1));
                long long i2 = static_cast<long long>(std::round(v2));
                if (i2 == 0) throw std::runtime_error("Modulo by zero.");
                value_stack.push(static_cast<double>(i1 % i2));
                break;
            }

            // Bitwise Binary Operators (require rounding)
            case OpCode::BIT_AND: {
                v2 = value_stack.top(); value_stack.pop();
                v1 = value_stack.top(); value_stack.pop();
                long long i1 = static_cast<long long>(std::round(v1));
                long long i2 = static_cast<long long>(std::round(v2));
                value_stack.push(static_cast<double>(i1 & i2));
                break;
            }
            case OpCode::BIT_OR: {
                v2 = value_stack.top(); value_stack.pop();
                v1 = value_stack.top(); value_stack.pop();
                long long i1 = static_cast<long long>(std::round(v1));
                long long i2 = static_cast<long long>(std::round(v2));
                value_stack.push(static_cast<double>(i1 | i2));
                break;
            }
            case OpCode::BIT_XOR: {
                v2 = value_stack.top(); value_stack.pop();
                v1 = value_stack.top(); value_stack.pop();
                long long i1 = static_cast<long long>(std::round(v1));
                long long i2 = static_cast<long long>(std::round(v2));
                value_stack.push(static_cast<double>(i1 ^ i2));
                break;
            }

            // Unary Operators/Functions
            case OpCode::NEGATE: v1 = value_stack.top(); value_stack.pop(); value_stack.push(-v1); break;
            case OpCode::FN_SIN: v1 = value_stack.top(); value_stack.pop(); value_stack.push(std::sin(v1)); break;
            case OpCode::FN_COS: v1 = value_stack.top(); value_stack.pop(); value_stack.push(std::cos(v1)); break;
            case OpCode::FN_TAN: v1 = value_stack.top(); value_stack.pop(); value_stack.push(std::tan(v1)); break;
            case OpCode::FN_ASIN: v1 = value_stack.top(); value_stack.pop(); value_stack.push(std::asin(v1)); break;
            case OpCode::FN_ACOS: v1 = value_stack.top(); value_stack.pop(); value_stack.push(std::acos(v1)); break;
            case OpCode::FN_ATAN: v1 = value_stack.top(); value_stack.pop(); value_stack.push(std::atan(v1)); break;
            case OpCode::FN_LOG: v1 = value_stack.top(); value_stack.pop(); value_stack.push(std::log(v1)); break; // natural log
            case OpCode::FN_LOG10: v1 = value_stack.top(); value_stack.pop(); value_stack.push(std::log10(v1)); break;
            case OpCode::FN_EXP: v1 = value_stack.top(); value_stack.pop(); value_stack.push(std::exp(v1)); break;
            case OpCode::FN_SQRT: v1 = value_stack.top(); value_stack.pop(); value_stack.push(std::sqrt(v1)); break;
            case OpCode::FN_ABS: v1 = value_stack.top(); value_stack.pop(); value_stack.push(std::abs(v1)); break;

            case OpCode::BIT_NOT: {
                v1 = value_stack.top(); value_stack.pop();
                long long i1 = static_cast<long long>(std::round(v1));
                value_stack.push(static_cast<double>(~i1));
                break;
            }

            default:
                throw std::runtime_error("Unknown instruction code.");
            }
        }

        if (value_stack.size() != 1) {
            throw std::runtime_error("Evaluation error: stack size is not 1 after execution.");
        }

        return value_stack.top();
    }
};


ExprCalculator::ExprCalculator(std::string_view expr) :
    impl_{std::make_unique<Impl>(expr)}
{}

ExprCalculator::~ExprCalculator() = default;

auto ExprCalculator::operator()(const VariableMap& variables) const -> double
{
    return (*impl_)(variables);
}

} // namespace common
