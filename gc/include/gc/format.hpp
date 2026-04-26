/** @file
 * @brief Umbrella header: std::formatter specializations for core gc types.
 *
 * Including this header makes EdgeInputEnd, EdgeOutputEnd, Edge, Type*, and
 * Value directly usable as {} arguments in std::format / throw_ calls.
 * Each specialization delegates to the type's operator<<.
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

// edge.hpp defines formatters for EdgeInputEnd, EdgeOutputEnd, Edge
#include "gc/edge.hpp"
// type.hpp defines formatter for const mpk::mix::value::Type*
#include "mpk/mix/value/type.hpp"
// value.hpp defines formatter for mpk::mix::value::Value
#include "mpk/mix/value/value.hpp"
