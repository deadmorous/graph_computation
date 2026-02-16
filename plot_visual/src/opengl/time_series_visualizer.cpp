/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "plot_visual/opengl/time_series_visualizer.hpp"

#include "gc_types/live_time_series.hpp"
#include "gc_types/palette.hpp"

#include "plot_visual/axis.hpp"
#include "plot_visual/axes_2d.hpp"
#include "plot_visual/color.hpp"
#include "plot_visual/layout.hpp"
#include "plot_visual/linear_coordinate_mapping.hpp"

#include <QColor>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QMatrix4x4>

#include <cmath>
#include <optional>
#include <vector>


namespace plot::opengl {

using namespace gc_types;

using CoordMap = plot::LinearCoordinateMapping<double, double>;
using PlotAxis = plot::Axis<CoordMap>;
using PlotAxes = plot::Axes2d<PlotAxis, PlotAxis>;
using PlotAxesPainter = plot::AxesPainter<PlotAxes>;

struct TimeSeriesVisualizer::Storage
{
    Storage(LiveTimeSeries& time_series, Attributes& attributes):
        time_series{time_series},
        attributes{attributes}
    {
        time_series.register_checkpoint(checkpoint);
    }

    LiveTimeSeries& time_series;
    const Attributes& attributes;
    LiveTimeSeries::Checkpoint checkpoint;

    QOpenGLWidget* ogl_widget{};
    OpenGLFunctions* ogl{};
    bool ogl_initialized{false};
    QOpenGLShaderProgram ogl_program;
    QOpenGLShaderProgram ogl_dashed_program;
    QOpenGLBuffer ogl_vbo;
    QOpenGLVertexArrayObject ogl_vao;
    QOpenGLBuffer ogl_ebo{QOpenGLBuffer::IndexBuffer};
    int ogl_mvp_location{-1};
    int ogl_color_location{-1};
    int ogl_dashed_mvp_location{-1};
    int ogl_dashed_color_location{-1};
    int ogl_dashed_scale_location{-1};

    size_t vbo_state_count{};
    size_t vbo_frame_capacity{};

    std::optional<PlotAxesPainter> axes_painter;
};

namespace {

auto prepare_ogl(TimeSeriesVisualizer::Storage& s) -> void
{
    if (s.ogl_initialized)
        return;

    s.ogl_initialized = true;

    static const char* vertex_shader_source = R"(
        #version 330 core
        layout (location = 0) in vec2 position;
        uniform mat4 mvp;
        void main()
        {
            gl_Position = mvp * vec4(position, 0.0, 1.0);
        }
    )";

    static const char* fragment_shader_source = R"(
        #version 330 core
        uniform vec4 line_color;
        out vec4 frag_color;
        void main()
        {
            frag_color = line_color;
        }
    )";

    s.ogl_program.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                          vertex_shader_source);
    s.ogl_program.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                          fragment_shader_source);
    s.ogl_program.link();

    s.ogl_mvp_location = s.ogl_program.uniformLocation("mvp");
    s.ogl_color_location = s.ogl_program.uniformLocation("line_color");

    static const char* dashed_vertex_shader_source = R"(
        #version 330 core
        layout (location = 0) in vec2 position;
        layout (location = 1) in vec2 aDistance;
        out float vDistance;
        uniform mat4 mvp;
        uniform vec2 uScale;
        void main()
        {
            gl_Position = mvp * vec4(position, 0.0, 1.0);
            vec2 d = uScale * aDistance;
            vDistance = d.x + d.y;
        }
    )";

    static const char* dashed_fragment_shader_source = R"(
        #version 330 core
        in float vDistance;
        uniform vec4 line_color;
        out vec4 frag_color;
        void main()
        {
            const float uDashSize = 10.0;
            const float uGapSize = 10.0;
            if (mod(vDistance, uDashSize + uGapSize) > uDashSize)
                discard;
            frag_color = line_color;
        }
    )";

    s.ogl_dashed_program.addShaderFromSourceCode(
        QOpenGLShader::Vertex, dashed_vertex_shader_source);
    s.ogl_dashed_program.addShaderFromSourceCode(
        QOpenGLShader::Fragment, dashed_fragment_shader_source);
    s.ogl_dashed_program.link();

    s.ogl_dashed_mvp_location =
        s.ogl_dashed_program.uniformLocation("mvp");
    s.ogl_dashed_color_location =
        s.ogl_dashed_program.uniformLocation("line_color");
    s.ogl_dashed_scale_location =
        s.ogl_dashed_program.uniformLocation("uScale");
}

auto ensure_buffers(TimeSeriesVisualizer::Storage& s,
                    size_t state_count,
                    size_t frame_capacity) -> void
{
    if (s.vbo_state_count == state_count &&
        s.vbo_frame_capacity == frame_capacity)
        return;

    if (s.ogl_vao.isCreated()) s.ogl_vao.destroy();
    if (s.ogl_vbo.isCreated()) s.ogl_vbo.destroy();
    if (s.ogl_ebo.isCreated()) s.ogl_ebo.destroy();

    s.vbo_state_count = state_count;
    s.vbo_frame_capacity = frame_capacity;

    auto total_vertices = state_count * frame_capacity;
    auto vbo_bytes = static_cast<int>(total_vertices * 4 * sizeof(float));
    auto ebo_bytes = static_cast<int>(frame_capacity * sizeof(uint32_t));

    s.ogl_vao.create();
    s.ogl_vao.bind();

    s.ogl_vbo.create();
    s.ogl_vbo.bind();
    s.ogl_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    s.ogl_vbo.allocate(vbo_bytes);

    s.ogl->glEnableVertexAttribArray(0);
    s.ogl->glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    s.ogl->glEnableVertexAttribArray(1);
    s.ogl->glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
        reinterpret_cast<void*>(2 * sizeof(float)));

    s.ogl_ebo.create();
    s.ogl_ebo.bind();
    s.ogl_ebo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    s.ogl_ebo.allocate(ebo_bytes);

    s.ogl_vbo.release();
    s.ogl_vao.release();
}

auto upload_frames(TimeSeriesVisualizer::Storage& s,
                   const LiveTimeSeries::Frames& frames,
                   size_t state_count,
                   size_t frame_capacity,
                   size_t frames_to_upload) -> void
{
    s.ogl_vbo.bind();

    auto frame_count = frames.size();
    auto first_index = frame_count - frames_to_upload;

    for (size_t fi = first_index; fi < frame_count; ++fi)
    {
        const auto& frame = frames[fi];
        auto slot = frame.ordinal % frame_capacity;
        for (size_t state = 0; state < state_count; ++state)
        {
            float vertex[4] = {
                static_cast<float>(frame.ordinal),
                (state < frame.values.size())
                    ? static_cast<float>(frame.values[state])
                    : 0.0f,
                0.0f,
                0.0f
            };
            auto offset = static_cast<int>(
                (state * frame_capacity + slot) * 4 * sizeof(float));
            s.ogl_vbo.write(offset, vertex, sizeof(vertex));
        }
    }

    s.ogl_vbo.release();
}

auto set_viewport(TimeSeriesVisualizer::Storage& s, const QRect& rect) -> void
{
    auto dpr = s.ogl_widget->devicePixelRatioF();
    auto widget_height = static_cast<int>(s.ogl_widget->height() * dpr);

    s.ogl->glViewport(
        static_cast<int>(rect.x() * dpr),
        widget_height - static_cast<int>((rect.y() + rect.height()) * dpr),
        static_cast<int>(rect.width() * dpr),
        static_cast<int>(rect.height() * dpr));
}

auto upload_distances(TimeSeriesVisualizer::Storage& s,
                      const LiveTimeSeries::Frames& frames,
                      size_t state,
                      size_t frame_count,
                      size_t frame_capacity) -> void
{
    s.ogl_vbo.bind();

    float cum_dx = 0.0f;
    float cum_dy = 0.0f;

    for (size_t i = 0; i < frame_count; ++i)
    {
        const auto& frame = frames[i];
        auto slot = frame.ordinal % frame_capacity;

        if (i > 0)
        {
            const auto& prev = frames[i - 1];
            cum_dx += std::abs(
                static_cast<float>(frame.ordinal) -
                static_cast<float>(prev.ordinal));
            auto cur_val = (state < frame.values.size())
                ? static_cast<float>(frame.values[state]) : 0.0f;
            auto prev_val = (state < prev.values.size())
                ? static_cast<float>(prev.values[state]) : 0.0f;
            cum_dy += std::abs(cur_val - prev_val);
        }

        float dist[2] = { cum_dx, cum_dy };
        auto offset = static_cast<int>(
            (state * frame_capacity + slot) * 4 * sizeof(float)
            + 2 * sizeof(float));
        s.ogl_vbo.write(offset, dist, sizeof(dist));
    }

    s.ogl_vbo.release();
}

} // anonymous namespace

TimeSeriesVisualizer::~TimeSeriesVisualizer() = default;

TimeSeriesVisualizer::TimeSeriesVisualizer(LiveTimeSeries& time_series,
                                           Attributes& attributes) :
    storage_{std::make_unique<Storage>(time_series, attributes) }
{}

auto TimeSeriesVisualizer::bind_opengl_widget(QOpenGLWidget& ogl_widget,
                                              OpenGLFunctions& ogl)
    -> void
{
    auto& s = *storage_;
    s.ogl_widget = &ogl_widget;
    s.ogl = &ogl;
    s.ogl_initialized = false;
    s.vbo_state_count = 0;
    s.vbo_frame_capacity = 0;
}

auto TimeSeriesVisualizer::paint_3d(const QRect& rect) -> void
{
    auto& s = *storage_;
    prepare_ogl(s);

    auto frames = s.time_series.frames();

    // Clear the full rect to white
    set_viewport(s, rect);
    s.ogl->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    s.ogl->glClear(GL_COLOR_BUFFER_BIT);

    if (frames.empty())
    {
        s.axes_painter.reset();
        return;
    }

    auto frame_capacity = s.time_series.frame_capacity();
    auto frame_count = frames.size();
    auto state_count = frames.front().values.size();

    auto y_max_d = s.time_series.value_range().end;
    if (y_max_d <= 0.)
        y_max_d = 1.;

    // Construct AxesPainter for layout and 2D drawing
    s.axes_painter.emplace(
        PlotAxes{
            .x = {
                .mapping{
                    .from{
                        .begin = -static_cast<double>(frame_capacity),
                        .end = 0}},
                .label = s.attributes.x_label },
            .y = {
                .mapping{ .from = { .begin = 0., .end = y_max_d }},
                .label = s.attributes.y_label },
            .title = s.attributes.title
        },
        rect);

    auto rc = s.axes_painter->layout().rect(plot::layout::central);

    // Sync checkpoint to determine what needs uploading
    auto update = s.checkpoint.sync();
    auto full_rebuild = !update.frames_added.has_value();
    auto frames_added = update.frames_added.value_or(frame_count);

    // Ensure buffers are allocated for current dimensions
    if (state_count != s.vbo_state_count ||
        frame_capacity != s.vbo_frame_capacity)
    {
        ensure_buffers(s, state_count, frame_capacity);
        full_rebuild = true;
    }

    // Upload frame data to VBO
    auto frames_to_upload = full_rebuild ? frame_count : frames_added;
    if (frames_to_upload > 0)
        upload_frames(s, frames, state_count, frame_capacity, frames_to_upload);

    // Build EBO indices: frames in logical (oldest-to-newest) order
    {
        std::vector<uint32_t> indices(frame_count);
        for (size_t i = 0; i < frame_count; ++i)
            indices[i] =
                static_cast<uint32_t>(frames[i].ordinal % frame_capacity);

        s.ogl_vao.bind();
        s.ogl_ebo.bind();
        s.ogl_ebo.write(
            0, indices.data(),
            static_cast<int>(frame_count * sizeof(uint32_t)));
        s.ogl_vao.release();
    }

    // Set viewport to the central (plot data) area
    set_viewport(s, rc);

    // Compute MVP: map ordinal range to x, value range to y.
    // X scale is fixed: [-frame_capacity, 0] maps to the full width,
    // with 0 (current generation) at the right edge.
    // In the VBO, x = ordinal, so we map
    // [newest_ordinal - frame_capacity, newest_ordinal] to clip space.
    auto newest_ordinal = static_cast<float>(frames.back().ordinal);
    auto x_left = newest_ordinal - static_cast<float>(frame_capacity);
    auto x_right = newest_ordinal;

    auto y_max = static_cast<float>(y_max_d);

    QMatrix4x4 mvp;
    mvp.ortho(x_left, x_right, 0.0f, y_max, -1.0f, 1.0f);

    // Draw time series lines
    auto dpr = s.ogl_widget->devicePixelRatioF();
    s.ogl->glLineWidth(2.0f * dpr);
    s.ogl_vao.bind();

    constexpr auto lightness_threshold = 0.9;

    for (size_t state = 0; state < state_count; ++state)
    {
        auto gc_color =
            map_color(s.attributes.palette, static_cast<uint32_t>(state));
        auto qc = plot::qcolor(gc_color);
        auto lightness = qc.lightnessF();

        if (lightness > lightness_threshold)
        {
            // Dashed line with halved lightness (matching painter path)
            upload_distances(s, frames, state, frame_count, frame_capacity);

            lightness /= 2;
            qc = QColor::fromHslF(
                qc.hslHueF(), qc.hslSaturationF(), lightness);

            s.ogl_dashed_program.bind();
            s.ogl_dashed_program.setUniformValue(
                s.ogl_dashed_mvp_location, mvp);
            s.ogl_dashed_program.setUniformValue(
                s.ogl_dashed_color_location,
                static_cast<float>(qc.redF()),
                static_cast<float>(qc.greenF()),
                static_cast<float>(qc.blueF()),
                static_cast<float>(qc.alphaF()));
            s.ogl_dashed_program.setUniformValue(
                s.ogl_dashed_scale_location,
                static_cast<float>(rc.width() * dpr / (x_right - x_left)),
                static_cast<float>(rc.height() * dpr / y_max));

            s.ogl->glDrawElementsBaseVertex(
                GL_LINE_STRIP,
                static_cast<GLsizei>(frame_count),
                GL_UNSIGNED_INT,
                nullptr,
                static_cast<GLint>(state * frame_capacity));

            s.ogl_dashed_program.release();
        }
        else
        {
            // Solid line
            s.ogl_program.bind();
            s.ogl_program.setUniformValue(s.ogl_mvp_location, mvp);
            auto [r, g, b, a] = r_g_b_a(gc_color);
            s.ogl_program.setUniformValue(
                s.ogl_color_location,
                r.v / 255.0f, g.v / 255.0f, b.v / 255.0f, a.v / 255.0f);

            s.ogl->glDrawElementsBaseVertex(
                GL_LINE_STRIP,
                static_cast<GLsizei>(frame_count),
                GL_UNSIGNED_INT,
                nullptr,
                static_cast<GLint>(state * frame_capacity));

            s.ogl_program.release();
        }
    }

    s.ogl_vao.release();
}

auto TimeSeriesVisualizer::paint_2d(const QRect&, QPainter& painter) -> void
{
    auto& s = *storage_;
    if (s.axes_painter)
        s.axes_painter->draw(painter);
}

} // namespace plot::opengl
