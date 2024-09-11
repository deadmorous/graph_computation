#include "mainwindow.hpp"

#include "bitmap_view.hpp"


MainWindow::MainWindow(SequenceGeneratorInterface& gen,
                       PresentationInterface& presentation,
                       QWidget *parent)
    : QMainWindow(parent)
{
    setCentralWidget(new BitmapView{gen, presentation});
}

MainWindow::~MainWindow() {}
