#pragma once

#include "gc_app/image.hpp"

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const gc_app::Image& image,
               QWidget *parent = nullptr);
};
