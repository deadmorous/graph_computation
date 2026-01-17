/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QVector>
#include <QMatrix4x4>

class GLTimeSeriesPlot : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

public:
    static constexpr int POINTS_PER_PATH = 5;

    GLTimeSeriesPlot(QWidget *parent = nullptr);

    // Call this to add a new set of points (one for each path)
    void addNewData(const QVector<float>& newYValues);

protected:
    void initializeGL() override;

    void paintGL() override;

private:
    void applyNewData();

    QVector<float> newYValues_;

    QOpenGLShaderProgram m_program;
    GLuint m_vbo, m_vao;

    QOpenGLShaderProgram m_fr_program;
    GLuint m_fr_vbo, m_fr_vao;

    int m_headIndex = 0;
};
