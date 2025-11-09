/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_visual/evolution_controller.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>


EvolutionController::EvolutionController(
    const std::string& type,
    GraphBroker* broker,
    QWidget* parent) :
    QWidget{parent}
{
    auto* layout = new QHBoxLayout{this};
    auto add_widget = [layout](auto* widget)
    {
        layout->addWidget(widget);
        return widget;
    };

    auto* reset_button = add_widget(new QPushButton{"&reset"});

    auto* next_button = add_widget(new QPushButton{"&next"});

    auto* start_button = add_widget(new QPushButton{"st&art"});

    auto* stop_button = add_widget(new QPushButton{"sto&p"});

    auto* interval_label = add_widget(new QLabel("&interval"));
    interval_label->setAlignment(Qt::AlignRight);
    auto* interval_widget = add_widget(new QSpinBox{});
    interval_label->setBuddy(interval_widget);
    interval_widget->setMinimum(1);
    interval_widget->setMaximum(10000);
    interval_widget->setSingleStep(5);
    interval_widget->setValue(50);

    auto* skip_label = add_widget(new QLabel("s&kip"));
    skip_label->setAlignment(Qt::AlignRight);
    auto* skip_widget = add_widget(new QSpinBox{});
    skip_label->setBuddy(skip_widget);
    skip_widget->setMinimum(1);
    skip_widget->setMaximum(100);
    skip_widget->setSingleStep(1);
    skip_widget->setValue(1);

    // ---

    auto* timer = new QTimer{this};
    connect(
        timer,
        &QTimer::timeout,
        [broker, skip_widget]{
            broker->advance_evolution(skip_widget->value());
        });

    auto stop = [timer] {
        timer->stop();
    };

    auto start = [timer, interval_widget] {
        timer->start(interval_widget->value());
    };

    // ---

    connect(
        reset_button,
        &QPushButton::clicked,
        [stop, broker] {
            stop();
            broker->reset_evolution();
        });

    connect(
        next_button,
        &QPushButton::clicked,
        [stop, broker, skip_widget] {
            stop();
            broker->advance_evolution(skip_widget->value());
        });

    connect(start_button, &QPushButton::clicked, start);

    connect(stop_button, &QPushButton::clicked, stop);

    connect(
        interval_widget,
        &QSpinBox::valueChanged,
        [timer](int interval) {
            if (timer->isActive())
                timer->start(interval);
        });
}
