#include "mainwindow.hpp"

#include "bitmap_view.hpp"


#if 0 // TODO
MainWindow::MainWindow(SequenceGeneratorInterface& gen,
                       PresentationInterface& presentation,
                       QWidget *parent)
    : QMainWindow(parent)
{
    setCentralWidget(new BitmapView{gen, presentation});
}

MainWindow::~MainWindow() {}
#endif // 0
