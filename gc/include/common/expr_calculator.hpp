/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include <memory>
#include <string_view>
#include <unordered_map>


namespace common {

class ExprCalculator final
{
public:
    using VariableMap = std::unordered_map<std::string_view, double>;

    explicit ExprCalculator(std::string_view expr);

    ~ExprCalculator();

    auto operator()(const VariableMap& variables) const -> double;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace common
