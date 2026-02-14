Now we need to implement the actual live time series OpenGL visualizer in the TimeSeriesVisualizer class.
We will need to treat ogl_vbo as a ring buffer holding values for most-recent time frames,
same as stored in TimeSeriesVisualizer::Storage::time_series, which is logically
already a ring buffer representing a number of N most recent frames of M scalar time series.
You can inspect the public interface of LiveTimeSeries to see how it works.
Importantly, we won't be copying all data to ogl_vbo at every call to TimeSeriesVisualizer::paint_3d.
Instead, we will use TimeSeriesVisualizer::Storage::checkpoint to figure out how
many frames have been added since we last synced, and only add those frames (or
all frames if there was no previous checkpoint).

To figure out what we need to eventually paint, you can find two working examples of
time series visualization, both in file plot_visual/src/painter/time_series_visualizer.cpp
- the paint_full function does not consider any incremental drawing and does
  a simple thing by stroking a few path using QPainter.
- the paint_incremental function does incremental updates to an image and also
  uses QPainter. You can see how to use checkpoints there; please ignore the logic
  of cache invalidation due to Y range change - we won't do that in the new visualizer:
  we are going to fit scale on every paint, and our vertex buffer is not going
  to be invalidated due to value range change, as opposed to pixmap used 
  by paint_incremental.

So in general you will implement painting fa full frame, without caching any part
of the image, but vertex buffer will be updated according to checkpoint logic.
