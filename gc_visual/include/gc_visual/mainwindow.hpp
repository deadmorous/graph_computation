#pragma once

// #include "gc_app/image.hpp"

// #include "yaml-cpp/node/node.h"

#include "config_spec.hpp"

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // MainWindow(const gc_app::Image& image,
    //            QWidget *parent = nullptr);
    explicit MainWindow(const gc_visual::ConfigSpecification& spec,
                        QWidget *parent = nullptr);
};
