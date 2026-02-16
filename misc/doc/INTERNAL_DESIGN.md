# Internal Design

## Directory structure

```
graph_computation/
  3p/              Third-party: googletest, benchmark, magic_enum, quill, yaml-cpp
  cmake/           CMake helper modules (gc_build_lib_config.cmake)
  examples/        .gc config files + rule/colormap/image assets
    cell_aut/      Cellular automaton examples
    num/           Prime/Waring visualization examples
  gc/              Core library (graph model, types, values, computation engine, YAML parsing)
  gc_app/          Application nodes (cell_aut, num, util, visual) + type/node registries
  gc_cli/          Headless CLI binary
  gc_types/        Domain types (Color, Image, Palette, LiveTimeSeries, UintSize, ...)
  gc_visual/       Qt GUI application
  plot_visual/     Qt/OpenGL time-series and histogram visualization library
  sieve/           Image metrics analysis nodes
  agc_rt/          Activation graph runtime support (experimental)
  agc_app/         Activation graph node definitions (Mandelbrot, linspace, canvas, ...)
  agc_app_rt/      Runtime types for activation graph nodes
  agc_perf/        Performance benchmarks for activation graph
  lib_config/      Shared library exporting build config for JIT compilation
  scripts/         Miscellaneous scripts
```

## CMake targets and dependencies

```
gc_visual (executable)
  +-- gc_app::lib (static)
  |     +-- gc::lib (static) -- yaml-cpp, quill, dl
  |     +-- gc_types::lib (static)
  |           +-- gc::lib
  +-- sieve::lib (static)
  |     +-- gc_app::lib
  +-- plot_visual::lib (static)
  |     +-- gc::lib, gc_types::lib
  |     +-- Qt6::{Core,Gui,Widgets,OpenGL,OpenGLWidgets}
  +-- Qt6::Widgets
  +-- FFMpeg::Encoder

gc_cli (executable)
  +-- gc::lib
  +-- gc_app::lib

lib_config (SHARED)
  +-- gc::lib
```

All libraries are static except `lib_config`, which is shared because it is
designed to be dynamically loaded at runtime to pass compiler/linker flags into
JIT-compiled activation graph code.

## Core library: `gc::lib`

Located in `gc/`. This is the heart of the engine.

### `gc::Type`

A compact, interned type descriptor stored in a fixed-size (64-byte) array.
Types are singleton-interned: equality is a byte comparison on the storage.
The type hierarchy is:

- `Scalar` (bool, byte, i8-i64, u8-u64, f32, f64)
- `String` (std::string, std::string_view)
- `Array<T,N>`, `Vector<T>`, `Tuple<Ts...>`, `Struct` (named tuple), `Set`
- `Strong` (strong typedef wrapper), `Enum`, `Custom` (opaque registered type),
  `Path`

A visitor pattern `gc::visit(type, f)` dispatches to typed wrappers
(`ScalarT`, `StructT`, `VectorT`, etc.).

### `gc::Value`

A type-erased value container: `const Type*` + `std::any`. Provides:

- Type-safe access via `as<T>()` (throws on mismatch).
- Structural navigation: `get(path)`, `set(path, v)`, `size(path)`,
  `resize(path, n)`, `keys()`, `contains()`, `insert()`, `remove()`.
- Numeric/string conversion via `convert_to<T>()`.

### `gc::ValuePath`

A path object (e.g. `"/color_map/0"`) for drilling into nested values by field
name or array index.

### `gc::ComputationNode`

The abstract base class for all compute nodes:

```cpp
struct ComputationNode {
    virtual auto input_names() const -> InputNames = 0;
    virtual auto output_names() const -> OutputNames = 0;
    virtual auto default_inputs(InputValues result) const -> void = 0;
    virtual auto compute_outputs(
        OutputValues result,
        ConstInputValues inputs,
        const std::stop_token& stoken,
        const NodeProgress& progress) const -> bool = 0;
};
```

`InputValues` / `OutputValues` are strongly-typed spans of `gc::Value` indexed
by `InputPort` / `OutputPort` (strong types wrapping `uint8_t`). Nodes check
`stop_token` for cancellation, report progress via the `NodeProgress` callback,
and return `false` if cancelled.

### `gc::Graph<Node, Edge>`

A container of `StrongVector<Node, NodeIndex>` + `std::vector<Edge>`.

### `gc::Edge`

`EdgeOutputEnd{NodeIndex, OutputPort}` to `EdgeInputEnd{NodeIndex, InputPort}`.

### `gc::SourceInputs`

External (non-edge) inputs: a `ValueVec` plus a grouped mapping from value
indices to `EdgeInputEnd` destinations.

### Compilation and execution

**`gc::compile(graph, provided_inputs)`** performs topological sort, validates
edge consistency (no cycles, no duplicate destinations, no out-of-range ports),
and builds `ComputationInstructions`: nodes grouped by level, edges grouped by
level, per-node upstream source sets.

**`gc::compute(result, graph, instructions, source_inputs, ...)`** executes the
computation. Uses timestamp-based incremental evaluation: a node is only
re-executed if any of its upstream sources changed since its last execution
timestamp.

### `gc::ComputationResult`

Holds input and output `Value` snapshots for all ports of all nodes
(`StrongGrouped<Value, NodeIndex, InputPort/OutputPort>`), per-node timestamps,
and an `updated_inputs` set for feedback-driven evolution.

### Registries

- **`gc::NodeRegistry<Node>`** = `ObjectRegistry<Node, ConstValueSpan,
  const Context<Node>&>` &mdash; maps string type names to factory functions.
- **`gc::TypeRegistry`** = `ValueRegistry<const Type*>` &mdash; maps type name
  strings to `const Type*`.
- **`gc::Context<Node>`** &mdash; pairs a `TypeRegistry` with a
  `NodeRegistry<Node>`.

### YAML parsing

`gc::yaml::parse_graph(yaml_node, context)` returns a `ParseGraphResult<Node>`
containing the populated graph, source inputs, a `NamedNodes<Node>` map, and a
list of named input strings. Nodes are instantiated via the registry. Edges are
parsed from `[from_node.port, to_node.port]` syntax. Inputs specify
type + value + destinations.

## Application nodes: `gc_app::lib`

Provides two registries populated at startup:

- `gc_app::populate_node_registry(registry)` &mdash; registers ~20
  `ComputationNode` factories by string name.
- `gc_app::populate_type_registry(registry)` &mdash; registers domain types.

All node factory functions follow the signature
`make_<name>(ConstValueSpan init_args, const ComputationContext&) ->
shared_ptr<ComputationNode>`. Init args (`init:` in YAML) allow passing
compile-time parameters to a node constructor (e.g. `merge` takes a count).

## Domain types: `gc_types::lib`

Shared data types used across modules:

- **`Color`** &mdash; strong `uint32_t`, ARGB format; decompose with
  `r_g_b_a()`.
- **`Image<Pixel>`** &mdash; `UintSize` + `std::vector<Pixel>`.
  Variants: `ColorImage`, `I8Image`, `U8Image`, etc.
- **`IndexedPalette`** &mdash; `vector<Color>` color_map + overflow_color.
- **`LiveTimeSeries`** &mdash; ring-buffer of frames with checkpoint-based
  incremental update tracking for visualization.

## GUI application: `gc_visual`

### Entry point

`gc_visual/src/main.cpp` starts `QApplication`, constructs `MainWindow` with
an embedded default `.gc` config, and calls `show()`.

### `MainWindow`

- Owns a `ComputationThread`.
- Menu: File (Open / Open Recent / Edit / Reload / Quit), Computation (Stop).
- `load(ConfigSpecification)`: parses YAML, populates context,
  calls `parse_graph()`, optionally `parse_graph_evolution()`, then
  `computation_thread_.set_graph(...)` and `start_computation()`.
  When the thread finishes: calls `parse_layout()` to build the central widget.
- Recent files stored in `QSettings` under `deadmorous/gc_visual`.
- Status bar shows elapsed computation time in seconds.

### `ComputationThread`

Extends `QThread`. Holds the `gc::Computation` (graph + instructions + source
inputs + result) and an optional `GraphEvolution` (feedback spec).

Thread body (`run()`): calls `gc::compute(computation_, stop_token,
progress_callback)`. In evolution mode, iterates `skip_+1` times calling
`set_feedback()` then `try_compute()` before `clear_feedback()`.

`set_feedback()` copies source output values to feedback destination inputs and
marks them as `updated_inputs` in the result.

Signals: `progress(NodeIndex, double)`, `running_state_changed(bool)`,
`computation_error(QString)`.

Cancellation: `stop_source_.request_stop()` then `wait()`.

### `GraphBroker`

The central mediator between the computation thread and UI widgets. Wraps
`ComputationThread&` + `NamedComputationNodes&`.

Provides `get_parameter()`, `get_port_value()`, `set_parameter()`,
`invalidate_input()`.

Emits `output_updated(EdgeOutputEnd)` when computation finishes so bound
widgets can refresh.

Uses `BindingResolver` to translate YAML binding strings
(e.g. `"image_colorizer"`, `"image_size/width"`) into `ParameterSpec` / `IoSpec`.

### Layout parsing

`parse_layout()` recursively processes the YAML `layout` section:

- **Containers**: `horizontal_layout` -> `QHBoxLayout`,
  `vertical_layout` -> `QVBoxLayout`, `stretch`.
- **Parameter editors** (via `GraphParameterEditor::supports_type()`): `spin`,
  `color`, `vector`, `file`, `flags`, `cell2d_rules`, `cell2d_gen_rules`,
  `cell2d_gen_cmap`, `list`.
- **Output visualizers** (via `GraphOutputVisualizer::supports_type()`):
  `image`, `image_metrics`, `text`.
- **Special**: `evolution` -> `EvolutionController`,
  `invalidate` -> `InvalidateButton`.

### Video recording

`VideoRecorder` uses FFmpeg to encode frames as H.264 MP4. The image
visualizer feeds frames to it.

## Visualization library: `plot_visual::lib`

Time-series chart rendering with two backend flavors:

### Painter backend (`plot_visual/src/painter/`)

- `TimeSeriesVisualizer` &mdash; draws line charts using `QPainter` with
  incremental update support.
- `TimeSeriesHistogramVisualizer` &mdash; histogram rendering.

### OpenGL backend (`plot_visual/src/opengl/`)

- `plot::opengl::TimeSeriesVisualizer` (extends `QOpenGLWidget`) &mdash;
  renders using OpenGL 3.3 core profile, with dashed-line support. Checkpoints
  `LiveTimeSeries` to detect new frames and uploads only incremental data to
  the GPU.
- `paint_3d(rect)` &mdash; OpenGL draw call.
- `paint_2d(rect, painter)` &mdash; axis labels and overlays via QPainter.

`plot::RenderingEngine` enum + `plot::rendering_engine()` factory select
between `PainterFull`, `PainterIncremental`, and `OpenGL` backends.

## Image metrics: `sieve::lib`

Extends `gc_app` with:

- `i8_image_metrics` node &mdash; computes `ImageMetrics` (state histogram,
  edge histogram, plateau average sizes) on an `I8Image`.
- `ImageMetricSet` &mdash; `EnumFlags<ImageMetric>` selecting which metrics to
  compute.

## Activation graph system (experimental): `agc_*`

A second computation model distinct from `ComputationNode`.

**`gc::ActivationNode`** describes a node whose computation is an algorithm
(an AST of C++ statements stored in `AlgorithmStorage`). Instead of pull-style
(supply all inputs, get all outputs), activation nodes are push-style: when an
input activates, it triggers an algorithm that may activate downstream outputs.

**`gc::AlgorithmStorage`** is an arena / interning store for algorithm AST
nodes: `Assign`, `Block`, `If`, `For`, `While`, `Do`, `FuncInvocation`,
`OutputActivation`, `Symbol`, `Type`, `Var`, etc.

**Code generation pipeline:**

1. Define activation nodes with algorithms in `agc_app`.
2. `generate_source(ostream, graph, alg_storage, ...)` emits a `.cpp` file.
3. `build::build()` invokes the system C++ compiler (with flags from
   `lib_config`).
4. Load and execute the resulting shared library.

`agc_perf` benchmarks this approach against reference implementations
using Google Benchmark.

## Threading model

| Thread | Responsibility |
|---|---|
| Main thread | Qt event loop, UI widgets, `GraphBroker`, layout parsing |
| Computation thread | `gc::compute()` with `stop_token`; communicates back via Qt signals |
| Video recorder thread | FFmpeg encoding inside `VideoRecorder::run()` |
| Quill logging thread | (optional) drains the lock-free log queue |

There is no lock-based synchronization between the computation thread and the
main thread during execution. The thread is fully stopped and joined before any
state mutation.

## Key patterns and conventions

**Trailing return type syntax** everywhere: `auto foo() -> ReturnType`.

**PIMPL pattern** with `struct Storage` in `.cpp` files for Qt widgets, and
`struct Impl` with `unique_ptr<Impl>` for core types (e.g. `LiveTimeSeries`,
`AlgorithmStorage`).

**`common::FastPimpl<Impl, N>`** &mdash; fixed-size inline PIMPL
(stack-allocated, avoids heap allocation). Used in `LiveTimeSeries` iterators.

**Strong types** (`GCLIB_STRONG_TYPE` macro): `NodeIndex`, `InputPort`,
`OutputPort`, `Color`, `ColorComponent`, etc. Carry domain semantics and
prevent accidental type mixing.

**`common::StrongVector<T, Index>`** &mdash; a vector indexed by a strong index
type.

**`common::StrongGrouped<T, OuterIndex, InnerIndex>`** /
**`common::Grouped<T>`** &mdash; jagged arrays (vector of vectors). Used for
per-node port values, instruction levels, etc.

**`common::FuncRef<Sig>`** &mdash; non-owning callable reference (like
`std::function` without heap allocation). Used for progress callbacks and
activation function pointers.

**`common::StructType<T>`** / `GCLIB_STRUCT_TYPE` macro &mdash; makes a struct
reflectable (field names, tuple access) so `gc::Type::of<T>()` can build its
type descriptor and `gc::Value` can navigate fields by name/path.

**`GCLIB_REGISTER_CUSTOM_TYPE(T, id)`** /
**`GCLIB_REGISTER_ENUM_TYPE(T, id)`** &mdash; registers a C++ type with a
numeric ID so `gc::Type::of<T>()` produces the correct aggregate type.

**Logging**: `common/log.hpp` wraps either `quill` (async structured logging)
or a no-op, selected at compile time via `GRAPH_COMPUTATION_LOG_ENGINE`
(`GC_LOG_QUILL` or `GC_LOG_NONE`).
