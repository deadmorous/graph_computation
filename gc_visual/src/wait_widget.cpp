#include "gc_visual/wait_widget.hpp"

#include <QApplication>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include <chrono>


using namespace std::chrono_literals;


WaitWidget::WaitWidget(QMainWindow* main_window) :
    main_window_{ main_window },
    parent_proxy_{ new QWidget{ main_window_ } }
{
    parent_proxy_->hide();
    setParent( parent_proxy_ );

    auto layout = new QVBoxLayout{};
    setLayout(layout);

    auto cancel_button = new QPushButton{ tr("&Cancel") };

    layout->addWidget(cancel_button);

    hide();

    connect(cancel_button, &QPushButton::clicked,
            this, &WaitWidget::cancel_waiting);
}

auto WaitWidget::start_waiting()
    -> void
{
    if (is_waiting_)
        return;


    is_waiting_ = true;
    is_waiting_layout_ = false;

    QTimer::singleShot(500ms, this, &WaitWidget::switch_to_waiting_layout);
}

auto WaitWidget::switch_to_waiting_layout()
    -> void
{
    if (!is_waiting_)
        return;

    is_waiting_layout_ = true;

    focus_backup_ = QApplication::focusWidget();

    central_widget_backup_ = main_window_->centralWidget();

    if (central_widget_backup_)
    {
        central_widget_backup_->setParent(parent_proxy_);
        central_widget_backup_->hide();
    }
    main_window_->setCentralWidget(this);
    show();
}

auto WaitWidget::stop_waiting()
    -> void
{
    if (!is_waiting_)
        return;

    is_waiting_ = false;

    if (!is_waiting_layout_)
        return;

    is_waiting_layout_ = false;

    hide();
    setParent(parent_proxy_);

    if (central_widget_backup_)
    {
        main_window_->setCentralWidget(central_widget_backup_);
        central_widget_backup_->show();
        if (focus_backup_)
            focus_backup_->setFocus();
    }
}

auto WaitWidget::set_waiting(bool waiting)
    -> void
{
    if (waiting)
        start_waiting();
    else
        stop_waiting();
}
