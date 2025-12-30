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

#include "gc/type.hpp"


namespace gc_visual {

namespace {

class TypeFlattener final
{
public:
    explicit TypeFlattener(const gc::Type* type)
    {
        visit(type, *this);
    }

    operator TypeComponentVec() &&
    { return std::move(result_); }

    auto operator()(const gc::ArrayT& t)
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

    auto operator()(const gc::CustomT& t)
        -> void
    {
    }

    auto operator()(const gc::EnumT& t)
        -> void
    { result_.push_back({ t.type(), current_path_ }); }

    auto operator()(const gc::PathT& t)
        -> void
    { result_.push_back({ t.type(), current_path_ }); }

    auto operator()(const gc::ScalarT& t)
        -> void
    { result_.push_back({ t.type(), current_path_ }); }

    auto operator()(const gc::StringT& t)
        -> void
    { result_.push_back({ t.type(), current_path_ }); }

    auto operator()(const gc::StrongT& t)
        -> void
    { result_.push_back({ t.type(), current_path_ }); }

    auto operator()(const gc::StructT& t)
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

    auto operator()(const gc::TupleT& t)
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

    auto operator()(const gc::VectorT& t)
        -> void
    { common::throw_("flatten_type failed: nested vectors are not supported"); }


private:
    TypeComponentVec result_;

    gc::ValuePath current_path_;
};

} // anonymous namespace


auto flatten_type(const gc::Type* t)
    -> std::vector<TypeComponent>
{ return TypeFlattener{t}; }

} // namespace gc_visual
