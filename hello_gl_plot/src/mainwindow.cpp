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

#include "gl_plot_widget.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    auto plot_widget = new GLTimeSeriesPlot{};

    plot_widget->addNewData({
        0.,
        1.,
        4.,
        9.,
        16
    });

    setCentralWidget(plot_widget);
}
