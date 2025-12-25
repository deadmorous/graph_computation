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

#include "gc_visual/editors/parameter_editor_widget.hpp"

#include <yaml-cpp/node/node.h>

#include <memory>

class SpinEditorWidget final :
    public ParameterEditorWidget
{
    Q_OBJECT

public:
    explicit SpinEditorWidget(const YAML::Node& config,
                              QWidget* parent = nullptr);

    auto value() const -> gc::Value override;

    static auto check_type(const gc::Type*) -> TypeCheckResult;

public slots:
    void set_value(const gc::Value& value) override;

private:
    struct ValueProxy
    {
        virtual ~ValueProxy() = default;
        virtual auto set(const gc::Value&) -> void = 0;
        virtual auto get() -> gc::Value = 0;
    };

    auto maybe_construct(const gc::Value& value) -> void;

    YAML::Node config_;
    std::unique_ptr<ValueProxy> value_proxy_;
};
