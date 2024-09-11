#include "mainwindow.hpp"

#include "eratosthenes_sieve.hpp"
#include "simple_presentation.hpp"
#include "spiral_presentation.hpp"
#include "test_sequence.hpp"

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
