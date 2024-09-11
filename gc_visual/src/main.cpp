#include "gc_visual/mainwindow.hpp"

#include "gc_visual/simple_presentation.hpp"
#include "gc_visual/spiral_presentation.hpp"

#include "gc_app/eratosthenes_sieve.hpp"
#include "gc_app/test_sequence.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if 0 // TODO
    // TestSequence generator;
    EratosthenesSieve generator;

    SimplePresentation presentation;
    //SpiralPresentation presentation;

    MainWindow w(generator, presentation);
    w.show();
#endif // 0
    return a.exec();
}
