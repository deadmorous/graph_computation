#pragma once

#include "sequence_generator.hpp"
#include "presentation.hpp"

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(SequenceGeneratorInterface& gen,
               PresentationInterface& presentation,
               QWidget *parent = nullptr);
    ~MainWindow();
};
