/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "sieve/types/image_metrics.hpp"

#include "gc_app/types/palette.hpp"

#include <QImage>
#include <QWidget>

class ImageMetricsView
    : public QWidget
{
    Q_OBJECT

public:

    enum class Type : uint8_t
    {
        StateHistogram,
        EdgeHistogram
    };
    Q_ENUM(Type);

    explicit ImageMetricsView(size_t buf_size, QWidget* parent = nullptr);

    ~ImageMetricsView();

    auto type() const noexcept -> Type;

public slots:
    auto add_image_metrics(const sieve::ImageMetrics& image_metrics) -> void;
    auto clear() -> void;
    auto set_palette(const gc_app::IndexedPalette&) -> void;
    auto set_type(Type) -> void;

protected:
    auto paintEvent(QPaintEvent*) -> void override;

public:
    struct Storage;

private:
    std::unique_ptr<Storage> storage_;
};

Q_DECLARE_METATYPE(ImageMetricsView::Type);
