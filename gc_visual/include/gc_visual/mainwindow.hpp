#pragma once

#include "gc/graph_computation.hpp"

#include "config_spec.hpp"

#include <QMainWindow>

class GraphBroker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // MainWindow(const gc_app::Image& image,
    //            QWidget *parent = nullptr);
    explicit MainWindow(const gc_visual::ConfigSpecification& spec,
                        QWidget *parent = nullptr);

private:
    auto load(const gc_visual::ConfigSpecification& spec)
        -> void;

    gc::Computation computation_;
    GraphBroker* graph_broker_;
};
