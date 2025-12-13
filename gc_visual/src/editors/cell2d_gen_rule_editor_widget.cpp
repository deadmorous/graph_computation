/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/editors/cell2d_gen_rule_editor_widget.hpp"

#include "gc_visual/editors/vector_editor_widget.hpp"

#include "gc_app/nodes/cell_aut/generate_rules.hpp"
#include "gc_app/types/cell2d_gen_rules.hpp"
#include "gc_app/types/cell2d_rules.hpp"

#include "gc/computation_context.hpp"
#include "gc/value.hpp"

#include "common/func_ref.hpp"
#include "common/scoped_inc.hpp"

#include <yaml-cpp/yaml.h>

#include <QBoxLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QSpinBox>

struct Cell2dGenRuleEditorWidget::Storage
{
    struct GenRuleMapWidgets final
    {
        QGroupBox* group;
        QLineEdit* formula{};
        VectorEditorWidget* overlays{};

        auto setVisible(bool visible) -> void
        {
            group->setVisible(visible);
        }
    };

    gc_app::Cell2dGenRules gen_rules;

    QSpinBox* state_count{};
    QSpinBox* min_state{};
    QCheckBox* tor{};
    QCheckBox* count_self{};
    GenRuleMapWidgets map9;
    GenRuleMapWidgets map6;
    GenRuleMapWidgets map4;
    QLabel* rule_view;

    int in_set_value{};

    std::shared_ptr<gc::ComputationNode> rule_generator;
    std::optional<gc_app::Cell2dRules> rules;
    std::string rule_gen_error;
};

namespace {

auto generate_rules(Cell2dGenRuleEditorWidget::Storage& s) -> void
{
    gc::ValueVec inputs{ s.gen_rules };
    gc::ValueVec outputs(1);
    try {
        s.rule_generator->compute_outputs(outputs, inputs, {}, {});
        s.rules = outputs[0].as<gc_app::Cell2dRules>();
        s.rule_gen_error.clear();
    }
    catch(std::exception& e)
    {
        s.rules.reset();
        s.rule_gen_error = e.what();
    }
}

auto rules_pixmap(Cell2dGenRuleEditorWidget::Storage& s) -> QPixmap
{
    auto& rules = s.rules;
    constexpr auto pixmap_size = QSize{512, 256};
    constexpr auto pixmap_rc = QRect{QPoint{0, 0}, pixmap_size};
    auto image = QImage{pixmap_size, QImage::Format_ARGB32};
    QPainter p{ &image };
    p.fillRect(pixmap_rc, Qt::white);

    auto not_available = [&]
    {
        p.setPen(Qt::darkRed);
        auto text = s.rule_gen_error.empty()
            ? QString{"N/A"}
            : QString::fromUtf8(s.rule_gen_error);
        p.drawText(
            pixmap_rc,
            Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap,
            text);
        return QPixmap::fromImage(image);
    };

    if (!rules.has_value())
        return not_available();

    if (rules->map9.empty())
        return not_available();

    auto N = rules->map9.size();
    auto W = pixmap_size.width();
    auto subsample = std::max(1ul, (N+W-1) / W);
    auto plot_map = [&](const std::vector<int8_t>& m, QRgb rgb)
    {
        auto fade_color = QColor::fromRgb(rgb);
        {
            auto h = fade_color.hsvHueF();
            auto s = fade_color.hsvSaturationF();
            auto v = fade_color.valueF();
            constexpr auto saturation_fade_factor = 0.5;
            constexpr auto value_increase_factor = 0.7;
            s *= saturation_fade_factor;
            v = value_increase_factor + (1-value_increase_factor)*v;
            fade_color.setHsvF(h, s, v);
        }

        for (size_t x=0, n=m.size(); x<n; ++x)
        {
            auto px = x / subsample;
            if (m[x] == -128)
                p.fillRect(px, 0, 1, pixmap_size.height(), fade_color);
            else
            {
                auto py =
                    pixmap_size.height() - 1 - (int{m[x]} - rules->min_state);
                if (py >= 0 && py < pixmap_size.height())
                    image.setPixel(px, py, rgb);
            }
        }
    };

    plot_map(rules->map9, QColor{Qt::black}.rgba());
    if (!rules->tor)
    {
        plot_map(rules->map6, QColor{Qt::blue}.rgba());
        plot_map(rules->map4, QColor{Qt::red}.rgba());
    }
    return QPixmap::fromImage(image);
}

auto visualize_rules(Cell2dGenRuleEditorWidget::Storage& s) -> void
{
    generate_rules(s);
    s.rule_view->setPixmap(rules_pixmap(s));
}


} // anonymous namespace

Cell2dGenRuleEditorWidget::Cell2dGenRuleEditorWidget(const YAML::Node&,
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

    s.state_count = make_spin("state count", 2, 255);
    s.min_state = make_spin("min. state", -127, 0);

    sub_layout = new QHBoxLayout{};
    layout->addLayout(sub_layout);

    s.tor = new QCheckBox("tor");
    sub_layout->addWidget(s.tor);

    s.count_self = new QCheckBox("count self");
    sub_layout->addWidget(s.count_self);

    auto make_map = [&](QString title) -> Storage::GenRuleMapWidgets
    {
        auto* group = new QGroupBox{title};

        auto* group_layout = new QVBoxLayout{};
        group->setLayout(group_layout);

        auto* sub_layout = new QHBoxLayout{};
        group_layout->addLayout(sub_layout);

        auto* formula_label = new QLabel{"&Formula"};
        auto* formula = new QLineEdit{};
        formula_label->setBuddy(formula);
        sub_layout->addWidget(formula_label);
        sub_layout->addWidget(formula);

        auto* overlays_label = new QLabel{"&Overlays"};
        auto* overlays = new VectorEditorWidget{{}};
        overlays_label->setBuddy(overlays);
        group_layout->addWidget(overlays_label);
        group_layout->addWidget(overlays);

        layout->addWidget(group);

        return {
            .group = group,
            .formula = formula,
            .overlays = overlays
        };
    };

    s.map9 = make_map("map 9");
    s.map6 = make_map("map 6");
    s.map4 = make_map("map 4");

    s.rule_view = new QLabel{};
    layout->addWidget(s.rule_view);

    connect(s.state_count, &QSpinBox::valueChanged,
            [&](int value) {
                auto& s = *storage_;
                if (s.in_set_value > 0)
                    return;
                auto state_count = uint8_t(value);
                s.gen_rules.state_count = state_count;
                emit value_changed(s.gen_rules);
                visualize_rules(s);
            });

    connect(s.min_state, &QSpinBox::valueChanged,
            [&](int value) {
                auto& s = *storage_;
                if (s.in_set_value > 0)
                    return;
                s.gen_rules.min_state = int8_t(value);
                emit value_changed(s.gen_rules);
                visualize_rules(s);
            });

    connect(s.tor, &QCheckBox::clicked,
            [&](bool checked) {
                auto& s = *storage_;
                if (s.in_set_value > 0)
                    return;
                s.gen_rules.tor = checked;
                s.map6.setVisible(!checked);
                s.map4.setVisible(!checked);
                emit value_changed(s.gen_rules);
                visualize_rules(s);
            });

    connect(s.count_self, &QCheckBox::clicked,
            [&](bool checked) {
                auto& s = *storage_;
                if (s.in_set_value > 0)
                    return;
                s.gen_rules.count_self = checked;
                emit value_changed(s.gen_rules);
                visualize_rules(s);
            });

    auto connect_map = [&](Storage::GenRuleMapWidgets& widgets,
                           gc_app::Cell2dGenRules_Map& map)
    {
        QObject::connect(
            widgets.formula,
            &QLineEdit::textChanged,
            [this, &map](const QString& v)
            {
                auto& s = *storage_;
                if (s.in_set_value > 0)
                    return;
                map.formula = v.toUtf8().toStdString();
                emit value_changed(s.gen_rules);
                visualize_rules(s);
            });

        QObject::connect(
            widgets.overlays,
            &VectorEditorWidget::value_changed,
            [this, &map](const gc::Value& v, gc::ValuePathView path)
            {
                auto& s = *storage_;
                if (s.in_set_value > 0)
                    return;
                using Overlay = gc_app::Cell2dGenRules_Overlay;
                using OverlayVec = std::vector<Overlay>;
                auto dst = gc::Value{map.overlays};
                dst.set(path, v);
                map.overlays = dst.as<OverlayVec>();
                emit value_changed(s.gen_rules);
                visualize_rules(s);
            });
    };

    connect_map(s.map9, s.gen_rules.map9);
    connect_map(s.map6, s.gen_rules.map6);
    connect_map(s.map4, s.gen_rules.map4);

    s.rule_generator = gc_app::cell_aut::make_generate_rules({}, {});
}

Cell2dGenRuleEditorWidget::~Cell2dGenRuleEditorWidget() = default;

auto Cell2dGenRuleEditorWidget::value() const
    -> gc::Value
{ return storage_->gen_rules; }

auto Cell2dGenRuleEditorWidget::check_type(const gc::Type* type)
    -> TypeCheckResult
{
    static auto expected_type = gc::type_of<gc_app::Cell2dGenRules>();

    if (type == expected_type)
        return { .ok = true };

    return {
        .ok = false,
        .expected_type_description = common::format(expected_type)
    };
}

void Cell2dGenRuleEditorWidget::set_value(const gc::Value& v)
{
    auto& s = *storage_;
    auto inc_in_set_value = common::ScopedInc{s.in_set_value};

    auto gen_rules = v.as<gc_app::Cell2dGenRules>();
    if (s.gen_rules == gen_rules)
        return;

    s.gen_rules = gen_rules;

    s.state_count->setValue(s.gen_rules.state_count);
    s.min_state->setValue(s.gen_rules.min_state);
    s.tor->setChecked(s.gen_rules.tor);
    s.count_self->setChecked(s.gen_rules.count_self);

    auto set_map = [](Storage::GenRuleMapWidgets& widgets,
                      gc_app::Cell2dGenRules_Map& map)
    {
        widgets.formula->setText(QString::fromUtf8(map.formula));
        widgets.overlays->set_value(map.overlays);
    };
    set_map(s.map9, s.gen_rules.map9);
    set_map(s.map6, s.gen_rules.map6);
    set_map(s.map4, s.gen_rules.map4);

    s.map6.setVisible(!s.gen_rules.tor);
    s.map4.setVisible(!s.gen_rules.tor);

    visualize_rules(s);
}
