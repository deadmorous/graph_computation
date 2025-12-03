/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/editors/file_picker_widget.hpp"

#include "gc/value.hpp"

#include <yaml-cpp/yaml.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>


FilePickerWidget::FilePickerWidget(const YAML::Node& config,
                                   QWidget* parent) :
    ParameterEditorWidget{ parent },
    filter_{"All files (*)"}
{
    if (auto filter_node = config["filter"]; filter_node.IsDefined())
        filter_ = filter_node.as<std::string>();

    auto* layout = new QHBoxLayout{this};
    text_input_ = new QLineEdit{};
    layout->addWidget(text_input_);
    auto* button = new QPushButton("&...");
    layout->addWidget(button);

    QObject::connect(
        text_input_,
        &QLineEdit::textChanged,
        [this](const QString& v)
        {
            auto gc_val = gc::Value(v.toUtf8().toStdString());
            emit value_changed(gc_val);
        });

    QObject::connect(
        button, &QPushButton::clicked,
        this, &FilePickerWidget::open_file);

}

auto FilePickerWidget::value() const -> gc::Value
{
    return text_input_->text().toUtf8().toStdString();
}

auto FilePickerWidget::check_type(const gc::Type* type) -> TypeCheckResult
{
    if (type->aggregate_type() == gc::AggregateType::String)
        return { .ok = true };

    return {
        .ok = false,
        .expected_type_description = "a string type"
    };
}

void FilePickerWidget::set_value(const gc::Value& value)
{
    text_input_->setText(QString::fromUtf8(value.as<std::string>()));
}

void FilePickerWidget::open_file()
{
    auto f_info = QFileInfo{text_input_->text()};
    auto path = QString{};
    if (f_info.exists())
        path = f_info.filePath();
    else {
        auto parent_dir = f_info.dir();
        if (parent_dir.exists())
            path = parent_dir.path();
    }

    auto file_name = QFileDialog::getOpenFileName(
        this, "Open File",
        path,
        QString::fromUtf8(filter_));
    if (file_name.isEmpty())
        return;
    text_input_->setText(file_name);
};
