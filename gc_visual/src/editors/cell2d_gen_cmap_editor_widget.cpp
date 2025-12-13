/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/editors/cell2d_gen_cmap_editor_widget.hpp"

#include "gc_visual/editors/vector_editor_widget.hpp"

#include "gc_app/nodes/cell_aut/generate_cmap.hpp"
#include "gc_app/types/cell2d_gen_cmap.hpp"
#include "gc_app/types/palette.hpp"

#include "gc/computation_context.hpp"
#include "gc/value.hpp"

#include "common/func_ref.hpp"
#include "common/scoped_inc.hpp"

#include <yaml-cpp/yaml.h>

#include <QBoxLayout>
#include <QCheckBox>
#include <QFontMetrics>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QSpinBox>

struct Cell2dGenCmapEditorWidget::Storage
{
    gc_app::Cell2dGenCmap gen_cmap;

    QSpinBox* state_count{};
    QLineEdit* r_formula{};
    QLineEdit* g_formula{};
    QLineEdit* b_formula{};
    VectorEditorWidget* overlays{};
    QLabel* cmap_view;

    int in_set_value{};

    std::shared_ptr<gc::ComputationNode> cmap_generator;
    std::optional<gc_app::IndexedColorMap> cmap;
    std::string cmap_gen_error;
};

namespace {

auto generate_cmap(Cell2dGenCmapEditorWidget::Storage& s) -> void
{
    gc::ValueVec inputs{ s.gen_cmap };
    gc::ValueVec outputs(1);
    try {
        s.cmap_generator->compute_outputs(outputs, inputs, {}, {});
        s.cmap = outputs[0].as<gc_app::IndexedColorMap>();
        s.cmap_gen_error.clear();
    }
    catch(std::exception& e)
    {
        s.cmap.reset();
        s.cmap_gen_error = e.what();
    }
}

auto cmap_pixmap(Cell2dGenCmapEditorWidget::Storage& s) -> QPixmap
{
    auto& cmap = s.cmap;
    static int width = 512;
    static int height = QFontMetrics{QFont{}}.size(0, "X\nX").height();
    static const auto pixmap_size = QSize{width, height};
    static const auto pixmap_rc = QRect{QPoint{0, 0}, pixmap_size};
    auto image = QImage{pixmap_size, QImage::Format_ARGB32};
    QPainter p{ &image };
    p.fillRect(pixmap_rc, Qt::white);

    auto not_available = [&]
    {
        p.setPen(Qt::darkRed);
        auto text = s.cmap_gen_error.empty()
            ? QString{"N/A"}
            : QString::fromUtf8(s.cmap_gen_error);
        p.drawText(
            pixmap_rc,
            Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap,
            text);
        return QPixmap::fromImage(image);
    };

    if (!cmap.has_value())
        return not_available();

    const auto& cm = cmap.value();
    int n = cm.size();
    if (width >= n)
    {
        auto x1 = 0;
        for (int i=0; i<n; ++i)
        {
            auto [r, g, b, _] = r_g_b_a(cm[i]);
            auto rgb = qRgb(r.v, g.v, b.v);
            auto x2 = std::min(width, (i+1) * width / n);
            p.fillRect(x1, 0, x2-x1, height, rgb);
            x1 = x2;
        }
    }
    else
    {
        for (int x=0; x<width; ++x)
        {
            auto i = x * n / width;
            auto [r, g, b, _] = r_g_b_a(cm[i]);
            auto rgb = qRgb(r.v, g.v, b.v);
            p.fillRect(x, 0, x+1, height, rgb);
        }
    }
    return QPixmap::fromImage(image);
}

auto visualize_cmap(Cell2dGenCmapEditorWidget::Storage& s) -> void
{
    generate_cmap(s);
    s.cmap_view->setPixmap(cmap_pixmap(s));
}


} // anonymous namespace

Cell2dGenCmapEditorWidget::Cell2dGenCmapEditorWidget(const YAML::Node&,
                                               QWidget* parent) :
    ParameterEditorWidget{ parent },
    storage_{std::make_unique<Storage>()}
{
    auto& s = *storage_;

    auto* layout = new QVBoxLayout{};
    setLayout(layout);

    auto* sub_layout = new QHBoxLayout{};
    layout->addLayout(sub_layout);

    auto make_spin = [&](QString label_text, int min, int max)
    {
        auto* spin_label = new QLabel(label_text);
        sub_layout->addWidget(spin_label);
        auto* spin = new QSpinBox{};
        spin->setMinimum(min);
        spin->setMaximum(max);
        sub_layout->addWidget(spin);
        spin_label->setBuddy(spin);
        return spin;
    };

    s.state_count = make_spin("state count", 2, 256);

    sub_layout = new QHBoxLayout{};
    layout->addLayout(sub_layout);

    auto make_formula_editor = [&](QString title) -> QLineEdit*
    {
        auto* formula_label = new QLabel{title};
        auto* formula = new QLineEdit{};
        formula_label->setBuddy(formula);
        sub_layout->addWidget(formula_label);
        sub_layout->addWidget(formula);
        return formula;
    };

    s.r_formula = make_formula_editor("&Red");
    s.g_formula = make_formula_editor("&Green");
    s.b_formula = make_formula_editor("&Blue");

    auto* overlays_label = new QLabel{"&Overlays"};
    s.overlays = new VectorEditorWidget{{}};
    overlays_label->setBuddy(s.overlays);
    layout->addWidget(overlays_label);
    layout->addWidget(s.overlays);

    s.cmap_view = new QLabel{};
    layout->addWidget(s.cmap_view);

    connect(s.state_count, &QSpinBox::valueChanged,
            [&](int value) {
                auto& s = *storage_;
                if (s.in_set_value > 0)
                    return;
                s.gen_cmap.state_count = value;
                emit value_changed(s.gen_cmap);
                visualize_cmap(s);
            });

    auto connect_formula = [&](QLineEdit* widget, std::string& map)
    {
        QObject::connect(
            widget,
            &QLineEdit::textChanged,
            [this, &map](const QString& v)
            {
                auto& s = *storage_;
                if (s.in_set_value > 0)
                    return;
                map = v.toUtf8().toStdString();
                emit value_changed(s.gen_cmap);
                visualize_cmap(s);
            });
    };

    connect_formula(s.r_formula, s.gen_cmap.formula.r);
    connect_formula(s.g_formula, s.gen_cmap.formula.g);
    connect_formula(s.b_formula, s.gen_cmap.formula.b);

    QObject::connect(
        s.overlays,
        &VectorEditorWidget::value_changed,
        [this](const gc::Value& v, gc::ValuePathView path)
        {
            auto& s = *storage_;
            if (s.in_set_value > 0)
                return;
            using Overlay = gc_app::Cell2dGenCmap_Overlay;
            using OverlayVec = std::vector<Overlay>;
            auto dst = gc::Value{s.gen_cmap.overlays};
            dst.set(path, v);
            s.gen_cmap.overlays = dst.as<OverlayVec>();
            emit value_changed(s.gen_cmap);
            visualize_cmap(s);
        });

    s.cmap_generator = gc_app::cell_aut::make_generate_cmap({}, {});
}

Cell2dGenCmapEditorWidget::~Cell2dGenCmapEditorWidget() = default;

auto Cell2dGenCmapEditorWidget::value() const
    -> gc::Value
{ return storage_->gen_cmap; }

auto Cell2dGenCmapEditorWidget::check_type(const gc::Type* type)
    -> TypeCheckResult
{
    static auto expected_type = gc::type_of<gc_app::Cell2dGenCmap>();

    if (type == expected_type)
        return { .ok = true };

    return {
        .ok = false,
        .expected_type_description = common::format(expected_type)
    };
}

void Cell2dGenCmapEditorWidget::set_value(const gc::Value& v)
{
    auto& s = *storage_;
    auto inc_in_set_value = common::ScopedInc{s.in_set_value};

    auto gen_cmap = v.as<gc_app::Cell2dGenCmap>();
    if (s.gen_cmap == gen_cmap)
        return;

    s.gen_cmap = gen_cmap;

    s.state_count->setValue(s.gen_cmap.state_count);
    s.r_formula->setText(QString::fromUtf8(s.gen_cmap.formula.r));
    s.g_formula->setText(QString::fromUtf8(s.gen_cmap.formula.g));
    s.b_formula->setText(QString::fromUtf8(s.gen_cmap.formula.b));
    s.overlays->set_value(s.gen_cmap.overlays);

    visualize_cmap(s);
}
