/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include <QPainter>

#include "gl_plot_widget.hpp"

GLTimeSeriesPlot::GLTimeSeriesPlot(QWidget *parent) :
    QOpenGLWidget(parent)
{
    setMinimumSize(500, 500);
}

    // Call this to add a new set of points (one for each path)
void GLTimeSeriesPlot::addNewData(const QVector<float>& newYValues) {
    newYValues_ = newYValues;
    update(); // Trigger repaint
}

void GLTimeSeriesPlot::applyNewData() {
    if (newYValues_.empty())
        return;


    // makeCurrent();
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, POINTS_PER_PATH*sizeof(float), newYValues_.data());
    // doneCurrent();
    newYValues_.clear();
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    float fr_data[] = {
       -4.f, 0.f, 0.f, 0.f,
        4.f, 0.f, 8.f, 0.f,
        4.f, 16.f, 8.f, 16.f,
       -4.f, 16.f, 16.f, 16.f,
       -4.f, 0.f, 16.f, 32.f,
    };
    glBindBuffer(GL_ARRAY_BUFFER, m_fr_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 20*sizeof(float), fr_data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLTimeSeriesPlot::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Shaders
    const char *vsrc = R"(
        #version 330 core
        layout (location = 0) in float yVal;
        uniform int uMaxPoints;
        uniform mat4 uProjection;
        void main() {
            // Calculate relative X: 0 is oldest, uMaxPoints-1 is newest
            int localIdx = gl_VertexID % uMaxPoints;
            float x = float(localIdx);
            gl_Position = uProjection * vec4(x, yVal, 0.0, 1.0);
            // gl_Position = vec4(x/10, x*x/20, 0, 1.0);
            // gl_Position = vec4(x/10, yVal/20, 0, 1.0);
        }
    )";

    const char *fsrc = R"(
        #version 330 core
        out vec4 fragColor;
        void main() { fragColor = vec4(0.0, 1.0, 0.0, 0.8); }
    )";

    m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
    m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, fsrc);
    m_program.link();

    // Buffer Setup: One big VBO for all data
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, POINTS_PER_PATH * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);    
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);



    const char *fr_vsrc = R"(
        #version 330 core
        layout (location = 0) in vec2 pos;
        layout (location = 1) in vec2 aDistance;
        out float vDistance; // Pass to fragment shader
        uniform mat4 uProjection;
        uniform vec2 uScale;
        void main() {
            gl_Position = uProjection * vec4(pos.x, pos.y, 0.0, 1.0);
            vec2 d = uScale * aDistance;
            vDistance = d.x + d.y;
        }
    )";

    const char *fr_fsrc = R"(
        #version 330 core
        in float vDistance;
        out vec4 fragColor;
        void main() {
            const float uDashSize = 10.0;
            const float uGapSize = 10.0;

            // float debugVal = vDistance / 500.0; // Scale it so it's visible (0 to 1)
            // fragColor = vec4(debugVal, 0.0, 0.0, 1.0); // If the line turns redder as it goes, math is correct

            if (mod(vDistance, uDashSize + uGapSize) > uDashSize) {
                discard;
            }
            fragColor = vec4(1.0, 0.0, 0.0, 0.8);
        }
    )";

    m_fr_program.addShaderFromSourceCode(QOpenGLShader::Vertex, fr_vsrc);
    m_fr_program.addShaderFromSourceCode(QOpenGLShader::Fragment, fr_fsrc);
    m_fr_program.link();

    glGenBuffers(1, &m_fr_vbo);
    glGenVertexArrays(1, &m_fr_vao);

    glBindVertexArray(m_fr_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_fr_vbo);
    glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLTimeSeriesPlot::paintGL() {
    struct L
    {
        qreal dpr;
        QRect r;
        QRect v;
        QRectF sr;
        QRectF sv;
        double sv_y() const {
            return (r.height() - (v.top() + v.height())) * dpr;
        }
    };

    auto l = [&]() -> L
    {
        constexpr auto l_margin = 30;
        constexpr auto r_margin = 20;
        constexpr auto b_margin = 10;
        constexpr auto t_margin = 15;
        auto dpr = devicePixelRatioF();
        auto r = rect();
        auto v = r.adjusted(l_margin, t_margin, -r_margin, -b_margin);
        auto scaled_rect = [&](const QRect& r) -> QRectF
        {
            return {
                r.left() * dpr,
                r.top() * dpr,
                r.width() * dpr,
                r.height() * dpr
            };
        };
        return { dpr, r, v, scaled_rect(r), scaled_rect(v) };
    }();

    applyNewData();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glViewport(viewport_x, viewport_y, viewport_width, viewport_height);
    glEnable(GL_SCISSOR_TEST);
    glScissor(l.sv.left(), l.sv_y(), l.sv.width(), l.sv.height());
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f); // Light grey
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST); // Disable it so it doesn't interfere with other draws later

    auto xmin = -4.f;
    auto xmax = 4.f;
    auto ymin = 0.f;
    auto ymax = 16.f;

    QMatrix4x4 projection;
    [&]() {
        auto d = l.sv.center() - l.sr.center();
        projection.scale(2./l.sr.width(), 2./l.sr.height());
        projection.translate(d.x(), -d.y());
        projection.scale(l.sr.width()/2., l.sr.height()/2.);
    }();

    projection.scale(2./(xmax-xmin), 2./(ymax-ymin));
    projection.scale(l.sv.width() / l.sr.width(),
                     l.sv.height() / l.sr.height());
    projection.translate(-0.5*(xmin+xmax), -0.5*(ymin+ymax));

    m_program.bind();
    m_program.setUniformValue("uProjection", projection);
    m_program.setUniformValue("uMaxPoints", POINTS_PER_PATH);

    glBindVertexArray(m_vao);
    glLineWidth(2.f);
    glDrawArrays(GL_LINE_STRIP, 0, POINTS_PER_PATH);

    m_fr_program.bind();
    m_fr_program.setUniformValue("uProjection", projection);
    m_fr_program.setUniformValue(
        "uScale", QSizeF{l.r.width()/(xmax-xmin), l.r.height()/(ymax-ymin)});
    glBindVertexArray(m_fr_vao);
    glLineWidth(1.f);
    glDrawArrays(GL_LINE_STRIP, 0, 5);

    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // ---

    auto painter = QPainter{this};
    painter.drawText(rect(), Qt::AlignHCenter | Qt::AlignVCenter, "Hello");
    painter.end();
}
