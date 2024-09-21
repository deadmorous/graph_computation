#include "gc_visual/mainwindow.hpp"

#include "gc_visual/bitmap_view.hpp"


MainWindow::MainWindow(const gc_app::Image& image,
                       QWidget *parent)
    : QMainWindow(parent)
{
    setCentralWidget(new BitmapView{image});
}
