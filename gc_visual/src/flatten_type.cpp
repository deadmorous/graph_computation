/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024-2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/flatten_type.hpp"

#include "mpk/mix/value/type.hpp"


namespace gc_visual {

namespace {

class TypeFlattener final
{
public:
    explicit TypeFlattener(const mpk::mix::value::Type* type)
    {
        visit(type, *this);
    }

    operator TypeComponentVec() &&
    { return std::move(result_); }

    auto operator()(const mpk::mix::value::ArrayT& t)
        -> void
    {
        const auto* element_type = t.element_type();
        auto n = t.element_count();
        auto root = std::move(current_path_);
        for (uint8_t i=0; i<n; ++i)
        {
            current_path_ = root / i;
            visit(element_type, *this);
        }
        current_path_ = std::move(root);
    }

    auto operator()(const mpk::mix::value::CustomT&)
        -> void
    {
    }

    auto operator()(const mpk::mix::value::EnumT& t)
        -> void
    { result_.push_back({ t.type(), current_path_ }); }

    auto operator()(const mpk::mix::value::PathT& t)
        -> void
    { result_.push_back({ t.type(), current_path_ }); }

    auto operator()(const mpk::mix::value::ScalarT& t)
        -> void
    { result_.push_back({ t.type(), current_path_ }); }

    auto operator()(const mpk::mix::value::SetT&)
        -> void
    { mpk::mix::throw_("flatten_type failed: nested sets are not supported"); }

    auto operator()(const mpk::mix::value::StringT& t)
        -> void
    { result_.push_back({ t.type(), current_path_ }); }

    auto operator()(const mpk::mix::value::StrongT& t)
        -> void
    { result_.push_back({ t.type(), current_path_ }); }

    auto operator()(const mpk::mix::value::StructT& t)
        -> void
    {
        auto field_names = t.field_names();
        auto tuple = t.tuple();
        auto field_types = tuple.element_types();
        auto n = tuple.element_count();
        auto root = std::move(current_path_);
        for (uint8_t i=0; i<n; ++i)
        {
            current_path_ = root / field_names[i];
            visit(field_types[i], *this);
        }
        current_path_ = std::move(root);
    }

    auto operator()(const mpk::mix::value::TupleT& t)
        -> void
    {
        auto field_types = t.element_types();
        auto n = t.element_count();
        auto root = std::move(current_path_);
        for (uint8_t i=0; i<n; ++i)
        {
            current_path_ = root / i;
            visit(field_types[i], *this);
        }
        current_path_ = std::move(root);
    }

    auto operator()(const mpk::mix::value::VectorT&)
        -> void
    { mpk::mix::throw_("flatten_type failed: nested vectors are not supported"); }


private:
    TypeComponentVec result_;

    mpk::mix::value::ValuePath current_path_;
};

} // anonymous namespace


auto flatten_type(const mpk::mix::value::Type* t)
    -> std::vector<TypeComponent>
{ return TypeFlattener{t}; }

} // namespace gc_visual
