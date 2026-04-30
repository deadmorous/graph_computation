# graph_computation

A **C++20 dataflow computation engine** that executes directed acyclic graphs
(DAGs) described in YAML `.gc` files.

You define a computation as a graph of typed nodes connected by edges, supply
initial inputs, and the engine executes each node in topological order,
propagating outputs downstream. Two front-ends are included: a **Qt 6 GUI**
(`gc_visual`) for interactive exploration and a **headless CLI** (`gc_cli`).

---

## What it can do

- **React in real time** — adjust parameters via spin boxes, color pickers,
  file pickers, and vector editors; the engine recomputes only what changed
  (timestamp-based incremental evaluation).
- **Simulate cellular automata** — step through generations frame-by-frame or
  run them continuously; feedback edges copy outputs back to inputs each step.
- **Visualise number theory** — render prime numbers or Waring's problem
  solutions as rectangular bitmaps or Ulam-spiral images.
- **Measure cellular automaton evolution** — compute state histograms, edge
  histograms, and plateau sizes live on each frame with OpenGL-accelerated
  scrolling charts.
- **Record to video** — export evolution sequences to H.264 MP4 via FFmpeg.
- **Run headless** — pipe `.gc` files through `gc_cli` without a display.
- **Extend with new nodes** — register a `ComputationNode` subclass by string
  name; the YAML file picks it up by that name with no further wiring.

---

## Quick start

```bash
# Build (with Qt GUI)
cmake -S . -B build/release \
      -DCMAKE_BUILD_TYPE=Release \
      -DGRAPH_COMPUTATION_BUILD_VISUAL=ON \
      -DCMAKE_PREFIX_PATH=$(qmake -query QT_INSTALL_PREFIX)
cmake --build build/release -j$(nproc)

# Run the GUI with Game of Life (acorn seed)
./build/release/gc_visual/gc_visual examples/cell_aut/cell2d_life.gc

# Headless
./build/release/gc_cli/gc_cli examples/cell_aut/cell2d_life.gc
```

More examples in `examples/`:

| File | What it shows |
|------|---------------|
| `cell_aut/cell2d_life.gc` | Conway's Game of Life, configurable rules, PNG seed image |
| `cell_aut/fractal_fireworks.gc` | Fractal-filler rules from a single lit pixel |
| `cell_aut/cell2d_img_sieve.gc` | Cellular automaton + live histogram / plateau metrics |
| `num/waring_spiral_view.gc` | Waring's problem solutions on an Ulam spiral |
| `num/primes_spiral_view.gc` | Prime numbers on an Ulam spiral |
| `num/waring_rect_view.gc` | Waring's problem solutions as a rectangular bitmap |

---

## The `.gc` file format

A `.gc` file is a YAML document with three top-level sections.

### Minimal example — Game of Life from a random seed

```yaml
graph:
  nodes:
    - name: initial_image
      type: random_image
    - name: cell2d
      type: cell2d
    - name: colorizer
      type: image_colorizer

  edges:
    - [initial_image.image, cell2d.input_state]
    - [cell2d.output_state, colorizer.input_image]

  inputs:
    - name: image_size
      type: UintSize
      value: {width: 500, height: 500}
      destinations: [initial_image.size]
    - name: palette
      type: IndexedPalette
      value:
        color_map: [0xffffffff, 0xff5953ff]
        overflow_color: 0xff000000
      destinations: [colorizer.palette]

evolution:
  feedback:
    - source: cell2d.output_state
      sink: [cell2d.input_state]

layout:
  type: horizontal_layout
  items:
    - type: spin
      bind: image_size/width
      range: [1, 2000]
    - type: invalidate
      bind: image_size
      label: "&reset"
    - type: evolution
    - type: image
      bind: colorizer
```

### YAML reference

**`graph.nodes[]`** — `name` (unique identifier) + `type` (registered node
type) + optional `init` (compile-time constructor arguments).

**`graph.edges[]`** — `[from_node.port, to_node.port]`. Port name can be
omitted when a node has a single input or output.

**`graph.inputs[]`** — named external values with `type`, `value`, and
`destinations` (list of `node.port` targets). These are the parameters exposed
to the UI.

**`evolution.feedback[]`** — copies a `source` output back to one or more
`sink` inputs before each generation step.

**`layout`** — recursive tree of containers (`horizontal_layout`,
`vertical_layout`, `stretch`) and leaf widgets. Widgets use `bind` to reference
a named input or `node.port` output; `/` navigates into nested values
(e.g. `palette/overflow_color`).

---

## Built-in node types

### Cellular automata (`cell_aut`)

| Node | Description |
|------|-------------|
| `cell2d` | 2D CA with configurable multi-state rules and torus/rect topology |
| `life` | Conway's Game of Life |
| `random_image` | Randomised initial-state generator |
| `image_loader` | Load a PNG as initial state |
| `image_colorizer` | Map an `I8Image` to a `ColorImage` via an indexed palette |
| `rule_reader`, `gen_rule_reader` | Read rule files (`.rul`, `.gen`) from disk |
| `generate_cmap`, `gen_cmap_reader` | Colormap generation / reading |
| `offset_image` | Spatial offset transform |

### Numerical (`num`)

| Node | Description |
|------|-------------|
| `eratosthenes_sieve` | Prime sieve producing a sequence |
| `waring`, `waring_parallel` | Waring's problem solver |
| `filter_seq` | Filter a sequence by divisibility |
| `multiply` | Multiply two U32 values |
| `test_sequence` | Synthetic test-sequence generator |

### Utility (`util`)

| Node | Description |
|------|-------------|
| `project` | Extract a named sub-value from a composite type |
| `merge` | Merge multiple values into a composite type |
| `uint_size` | Construct a `UintSize` from width and height |

### Visual (`visual`)

| Node | Description |
|------|-------------|
| `rect_view` | Render a number sequence as a rectangular bitmap |
| `spiral_view` | Render a number sequence as an Ulam-spiral bitmap |

### Image metrics (`sieve`)

| Node | Description |
|------|-------------|
| `i8_image_metrics` | Compute state histogram, edge histogram, and plateau average size on each CA frame |

---

## UI widget types

### Parameter editors

| Widget | Description |
|--------|-------------|
| `spin` | Integer or float spin box with configurable range |
| `color` | ARGB color picker |
| `vector` | Editable list of colors or other values |
| `file` | File picker with extension filter |
| `cell2d_rules` | Dedicated editor for `Cell2dRules` |
| `cell2d_gen_rules` / `cell2d_gen_cmap` | Editors for generated rule / colormap types |
| `flags` | Checkbox set for enum flags |
| `list` | Generic list editor |
| `invalidate` | Button that forces re-computation (used as "reset" in evolution) |
| `evolution` | Step / run controller for feedback-driven simulation |

### Output visualizers

| Widget | Description |
|--------|-------------|
| `image` | Displays a `ColorImage`; supports optional blend mode |
| `image_metrics` | Live scrolling time-series charts (histogram, edge histogram, plateau size) |
| `text` | Displays a value as formatted text |

---

## Architecture overview

### Execution pipeline

1. YAML `.gc` file parsed into a typed graph of nodes and edges.
2. `gc::compile()` — topological sort → level grouping → `ComputationInstructions`.
3. `gc::compute()` — executes level-by-level; nodes skip re-execution when no
   upstream value changed (timestamp-based incremental evaluation).
4. Optional **evolution loop** — feedback edges copy outputs back to inputs for
   cellular-automaton stepping.
5. Qt GUI renders results; parameter changes trigger partial recomputation on a
   background thread with cancellation via `std::stop_token`.

### Computation model

**`gc::ComputationNode`** (pull-style, primary model) — nodes declare typed
ports; the engine supplies inputs and collects outputs:

```cpp
virtual auto compute_outputs(
    OutputValues result,
    ConstInputValues inputs,
    const std::stop_token&,
    const NodeProgress&) const -> bool;
```

Returns `false` if cancelled. Progress reported via `NodeProgress`
(`mpk::mix::FuncRef<void(double)>`).

**`gc::ActivationNode`** (push-style, experimental `agc_*`) — per-input
activation algorithms defined as C++ ASTs, emitted to `.cpp`, JIT-compiled by
the system compiler, and dynamically loaded. Used in Mandelbrot benchmarks.

### Key types

Types are provided by the **mpk_mix** library (`3p/mpk_mix`) under the
`mpk::mix::` and `mpk::mix::value::` namespaces:

| Type | Description |
|------|-------------|
| `mpk::mix::value::Type` | Interned type descriptor; use `mpk::mix::Type<T>`, `MPKMIX_STRUCT_TYPE` |
| `mpk::mix::value::Value` | Type-erased value; navigate with `get(path)`, `set(path, v)` |
| `mpk::mix::value::ValuePath` | String path into nested values, e.g. `"/color_map/0"` |
| `mpk::mix::Strong<Traits>` | Zero-overhead strong typedef; used for `NodeIndex`, `InputPort`, `OutputPort`, `Color`, … |
| `mpk::mix::StrongGrouped<T,O,I>` | Jagged array; core structure for per-node port values in results |
| `mpk::mix::FuncRef<Sig>` | Non-owning callable (no heap allocation) |
| `mpk::mix::FastPimpl<Impl,N>` | Fixed-size stack-allocated PIMPL |

### Module layout

| Directory | Role |
|-----------|------|
| `gc/` | Core: type system, value system, graph compile/compute, YAML parsing, JIT build infrastructure |
| `gc_app/` | Application `ComputationNode` implementations: `cell_aut/`, `num/`, `util/`, `visual/` |
| `gc_types/` | Domain types: `Color`, `Image<Pixel>`, `IndexedPalette`, `LiveTimeSeries` |
| `gc_visual/` | Qt 6 GUI: `MainWindow`, `ComputationThread`, `GraphBroker`, layout parser, parameter editors, output visualizers, video recording |
| `plot_visual/` | Time-series charts: QPainter backend and OpenGL 3.3 backend with incremental GPU upload |
| `sieve/` | Image metrics nodes (`i8_image_metrics`) |
| `agc_rt/`, `agc_app/`, `agc_app_rt/`, `agc_perf/` | Experimental activation-graph JIT code-gen path |
| `3p/` | Git submodules: mpk_mix, googletest, benchmark, magic_enum, quill, yaml-cpp |

### GUI threading

| Thread | Responsibility |
|--------|----------------|
| Main thread | Qt event loop, `GraphBroker` mediator, layout/widget construction |
| Computation thread | `gc::compute()` with `stop_token`; communicates back via Qt signals; fully stopped before any state mutation |
| Video recorder thread | FFmpeg H.264 MP4 encoding |

---

## Building

**Prerequisites (core):** C++20 compiler, CMake, yaml-cpp, libpng.  
**Prerequisites (GUI):** Qt 6 (Widgets, OpenGL, OpenGLWidgets), FFmpeg.  
**Optional:** quill (async logging; falls back to no-op if absent).

### Core + CLI only (no Qt required)

```bash
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
cmake --build build/release -j$(nproc)
```

### With the Qt GUI (`gc_visual`)

Pass `-DGRAPH_COMPUTATION_BUILD_VISUAL=ON` and point CMake at your Qt 6
installation. Use `qmake -query QT_INSTALL_PREFIX` to obtain the prefix —
making sure `qmake` refers to Qt 6. On systems using `qtchooser`, run
`qtchooser -l` to list available versions, then select the appropriate one
via `QT_SELECT`:

```bash
QT_SELECT=<qt6-version> cmake -S . -B build/release \
      -DCMAKE_BUILD_TYPE=Release \
      -DGRAPH_COMPUTATION_BUILD_VISUAL=ON \
      -DCMAKE_PREFIX_PATH=$(QT_SELECT=<qt6-version> qmake -query QT_INSTALL_PREFIX)
cmake --build build/release -j$(nproc)
```

On systems where `qmake` already refers to Qt 6:

```bash
cmake -S . -B build/release \
      -DCMAKE_BUILD_TYPE=Release \
      -DGRAPH_COMPUTATION_BUILD_VISUAL=ON \
      -DCMAKE_PREFIX_PATH=$(qmake -query QT_INSTALL_PREFIX)
cmake --build build/release -j$(nproc)
```

Key CMake options:

| Option | Default | Description |
|--------|---------|-------------|
| `GRAPH_COMPUTATION_BUILD_VISUAL` | `OFF` | Build the Qt GUI (`gc_visual`) and `plot_visual` |
| `GRAPH_COMPUTATION_SANITIZE_ADDRESS` | `OFF` | Enable `-fsanitize=address` |
| `GRAPH_COMPUTATION_LOG_ENGINE` | `GC_LOG_QUILL` | `GC_LOG_QUILL` or `GC_LOG_NONE` |

Warnings are treated as errors (`-Wall -Wextra -Werror`).

## Tests

```bash
ctest --test-dir build/release
# or directly:
./build/release/gc/test/gc-lib-test
./build/release/gc_app/test/gc_app-lib-test
./build/release/sieve/test/sieve-test
```

## Dependencies

| Library | Required for | Role |
|---------|-------------|------|
| yaml-cpp | core | `.gc` file parsing |
| libpng | core | PNG image loading |
| Qt 6 (Widgets, OpenGL, OpenGLWidgets) | GUI | GUI front-end |
| FFmpeg | GUI | Video recording |
| quill | optional | Async structured logging |
| magic_enum | bundled (`3p/`) | Enum reflection |
| mpk_mix | bundled (`3p/`) | Utility library: type system, value system, strong types, containers, logging |
| googletest / Google Benchmark | bundled (`3p/`) | Tests / benchmarks |
