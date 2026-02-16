Context
=======

## `plot_visual` visualizers for live time series objects.

We have a number of visualizers (or renderers) for time series and time series
histograms. Some of them are backed by QPainter, and some -
by OpenGL and QPainter. The former implement the `plot::painter::Visualizer`
interface (see `plot_visual/include/plot_visual/painter/visualizer.hpp`), and
the latter - the `plot::opengl::Visualizer` interface (see
`plot_visual/include/plot_visual/opengl/visualizer.hpp`). Types of renderers
are encoded in `TimeSeriesRenderer` and `TimeSeriesHistogramRenderer` enumerated
types (see `plot_visual/include/plot_visual/time_series_renderer.hpp`),
and "rendering engines" - in the `RenderingEngine` enumerated type (see
`plot_visual/include/plot_visual/rendering_engine.hpp`).

A specific visualizer is defined by its class type, which is one of
- `plot::painter::TimeSeriesVisualizer`,
- `plot::painter::TimeSeriesHistogramVisualizer`,
- `plot::opengl::TimeSeriesVisualizer`,

and, when full/incremetal options are available, by the `incremental` flag
in the `Attributes` struct.

## `gc_visual` Image metrics visualizer

There are `ImageMetricsVisualizer` and `ImageMetricsView` classes in the
`gc_visual` project - see source files
`gc_visual/src/visualizers/image_metrics_visualizer.cpp`,
`gc_visual/src/widgets/image_metrics_view.cpp`
and `image_metrics_visualizer.hpp`, `image_metrics_view.hpp` header files
they include.

`ImageMetricsVisualizer` controls which metric needs to be visualized, and
`ImageMetricsView` is a view where metrics are shown.

Notice that the `MyOpenGLWidget` class is a helper that I used to test OpenGL
renderer for time series. It needs not be there in the final version, at least
the same way as it's now; but we can reuse whatever useful is in there.

# The task

Items below should be done one by one, resulting in a separate commit each.
Include this document in the first commit.

## Allow user to optionally specify metric renderer

`ImageMetricsVisualizer::ImageMetricsVisualizer` is given a YAML node,
`item_node` with metrics visualizer parameters. It currently has
only the `lookback` parameter.
We need to add another parameter, a dictionary that maps `sieve::ImageMetric`
(see `sieve/include/sieve/types/image_metrics.hpp`) to available renderer type.
Note that `StateHistogram` and `EdgeHistogram` correspond to
`TimeSeriesHistogramRenderer` (and there is just one renderer available),
and `PlateauAvgSize` corresponds to `TimeSeriesRenderer` (and there are three
renderers available). In the future, we will probably add more metrics and
renderers. When user does not specify a renderer, we'll use defaults:
- `TimeSeriesHistogramRenderer::PainterIncremental` for
  `StateHistogram` and `EdgeHistogram`;
- `TimeSeriesRenderer::OpenGL` for `PlateauAvgSize`.

The mapping would go to the `ImageMetricsView`'s constructor. Please suggest
a solution for type representing the mapping, as it's not trivial because
different key values originally correspond to different value types.

At this stage, we just pass additional information to `ImageMetricsView` and
do not make any use of it.

## Refactor `Attributes` struct in `plot_visual` visualizers

Each of classes `plot::painter::TimeSeriesVisualizer`,
`plot::painter::TimeSeriesHistogramVisualizer`,
`plot::opengl::TimeSeriesVisualizer`
have a nested `Attributes` struct. All these types share a lot, which we should
refactor - let's have a common type (suggest a name for it). 
If we get rid of the `incremental` field of
`plot::painter::TimeSeriesVisualizer::Attributes`, all types will be the same.

Let's pass that `incremental` as another constructor parameter to
`plot::painter::TimeSeriesVisualizer`, unless you have a better solution.

## Update `ImageMetricsView` to handle any kind of renderer.

The major thing here is that some renderers need a QWidget, whereas some other
renderers require QOpenGLWidget. Maybe the best solution is to have
`ImageMetricsView` as a placeholder widget, and it will create a child widget
(occupying the same area as `ImageMetricsView` itself) to serve as a painting
surface suitable for renderer of chosen type.

Notice that widget type can be derived from the result of a `rendering_engine`
function call (see `plot_visual/include/plot_visual/time_series_renderer.hpp`).
