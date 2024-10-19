#pragma once

#include "gc/graph_computation.hpp"

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

private:
    auto load(const gc_visual::ConfigSpecification& spec)
        -> bool;

    auto reload_recent_files_menu()
        -> void;

    gc::Computation computation_;
    gc_visual::ConfigSpecification spec_;
    QSignalMapper* recents_mapper_;
    QMenu* recent_files_;
};
