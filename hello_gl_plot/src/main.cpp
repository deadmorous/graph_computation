/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "mainwindow.hpp"

#include <QApplication>

#include <iostream>


using namespace std::string_view_literals;

auto run(int argc, char *argv[])
    -> int
{
    QApplication a(argc, argv);

    MainWindow w{};
    w.show();

    return a.exec();
}

int main(int argc, char *argv[])
{
    try
    {
        return run(argc, argv);
    }
    catch(std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
