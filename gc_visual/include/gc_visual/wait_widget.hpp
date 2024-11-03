#pragma once

#include <QWidget>
#include <QMainWindow>

class WaitWidget :
    public QWidget
{
    Q_OBJECT
public:
    explicit WaitWidget(QMainWindow* main_window);

signals:
    auto cancel_waiting() -> void;

public slots:
    auto start_waiting() -> void;
    auto stop_waiting() -> void;
    auto set_waiting(bool waiting) -> void;
    auto switch_to_waiting_layout() -> void;

private:

    QMainWindow* main_window_;
    QWidget* parent_proxy_;
    bool is_waiting_{false};
    bool is_waiting_layout_{false};
    QWidget* focus_backup_{nullptr};
    QWidget* central_widget_backup_{nullptr};
};
