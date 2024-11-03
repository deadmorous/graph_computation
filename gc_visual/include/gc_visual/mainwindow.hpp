#pragma once

#include "gc_visual/computation_thread.hpp"

#include "config_spec.hpp"

#include <QMainWindow>
#include <QSignalMapper>

class GraphBroker;
class QMenu;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // MainWindow(const gc_app::Image& image,
    //            QWidget *parent = nullptr);
    explicit MainWindow(const gc_visual::ConfigSpecification& spec,
                        QWidget *parent = nullptr);

private slots:
    auto open() -> void;
    auto open_recent(const QString& file_name) -> void;
    auto edit() -> void;
    auto reload() -> void;
    auto on_load_finished(const gc_visual::ConfigSpecification& spec) -> void;

signals:
    auto load_finished(const gc_visual::ConfigSpecification& spec) -> void;

private:
    auto load(const gc_visual::ConfigSpecification& spec) -> void;
    auto reload_recent_files_menu() -> void;

    ComputationThread computation_thread_;
    gc_visual::ConfigSpecification spec_;
    QSignalMapper* recents_mapper_;
    QMenu* recent_files_;
};
