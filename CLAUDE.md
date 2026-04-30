# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

Core + CLI (no Qt needed):
```bash
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
cmake --build build/release -j$(nproc)
```

With Qt GUI (`gc_visual`), point CMake at the Qt 6 prefix from `qmake -query QT_INSTALL_PREFIX`, ensuring `qmake` refers to Qt 6. On systems using `qtchooser`, use `qtchooser -l` to find the right version name, then set `QT_SELECT`:
```bash
QT_SELECT=<qt6-version> cmake -S . -B build/release \
      -DCMAKE_BUILD_TYPE=Release \
      -DGRAPH_COMPUTATION_BUILD_VISUAL=ON \
      -DCMAKE_PREFIX_PATH=$(QT_SELECT=<qt6-version> qmake -query QT_INSTALL_PREFIX)
cmake --build build/release -j$(nproc)
```

On systems where `qmake` already refers to Qt 6, `QT_SELECT` can be omitted.

Key CMake options:
- `GRAPH_COMPUTATION_BUILD_VISUAL` (OFF) — build `gc_visual` Qt GUI and `plot_visual`
- `GRAPH_COMPUTATION_SANITIZE_ADDRESS` (OFF) — enables `-fsanitize=address`
- `GRAPH_COMPUTATION_LOG_ENGINE` — `GC_LOG_QUILL` (default) or `GC_LOG_NONE`

Warnings are treated as errors (`CMAKE_COMPILE_WARNING_AS_ERROR ON`, `-Wall -Wextra`).

## Tests

```bash
ctest --test-dir build
# Or directly:
./build/gc/test/gc-lib-test
./build/gc_app/test/gc_app-lib-test
```

## Run

```bash
./build/gc_visual/gc_visual examples/cell_aut/cell2d_life.gc   # GUI
./build/gc_cli/gc_cli examples/cell_aut/cell2d_life.gc          # Headless
```

## Architecture

This is a **C++20 dataflow computation engine** that executes directed acyclic graphs (DAGs) described in YAML `.gc` files.

### Execution Pipeline

1. YAML `.gc` file parsed into a typed graph of nodes and edges (`gc/src/yaml/parse_graph.cpp`)
2. `gc::compile()` performs topological sort → level grouping → `ComputationInstructions`
3. `gc::compute()` executes level-by-level with **timestamp-based incremental evaluation** (nodes skip re-execution if no upstream change)
4. Optional **evolution loop**: feedback edges copy outputs back to inputs for cellular-automaton stepping (`ComputationThread::set_feedback()`)
5. Qt GUI renders results via layout declared in the YAML file

### Computation Model

**`gc::ComputationNode`** (pull-style, main model) — nodes declare typed ports; engine supplies inputs, collects outputs:
```cpp
virtual auto compute_outputs(OutputValues result, ConstInputValues inputs,
                             const std::stop_token&, const NodeProgress&) const -> bool;
```
Returns `false` if cancelled. Progress reported via `NodeProgress` (`mpk::mix::FuncRef<void(double)>`).

**`gc::ActivationNode`** (push-style, experimental `agc_*`) — per-input activation algorithms defined as C++ ASTs, emitted to `.cpp`, JIT-compiled by system compiler, dynamically loaded. Used in Mandelbrot benchmarks.

### Key Types

Types formerly in `gc::` / `common::` namespaces are now provided by the **mpk_mix** library (`3p/mpk_mix`) under the `mpk::mix::` and `mpk::mix::value::` namespaces:

- **`mpk::mix::value::Type`** — interned type descriptor. Use `mpk::mix::Type<T>`, `MPKMIX_STRUCT_TYPE`, `MPKMIX_REGISTER_CUSTOM_TYPE` macros.
- **`mpk::mix::value::Value`** — type-erased value. Path navigation via `get(path)`, `set(path, v)`.
- **`mpk::mix::value::ValuePath`** — string path for navigating nested values (e.g. `"/color_map/0"`).
- **`mpk::mix::Strong<Traits>`** / `MPKMIX_STRONG_TYPE` — zero-overhead strong typedefs. Used extensively: `NodeIndex`, `InputPort`, `OutputPort`, `Color`, etc.
- **`mpk::mix::StrongGrouped<T, OuterIndex, InnerIndex>`** — jagged arrays; core structure for per-node port values in computation results.
- **`mpk::mix::FuncRef<Sig>`** — non-owning callable (no heap allocation).
- **`mpk::mix::FastPimpl<Impl, N>`** — fixed-size stack-allocated PIMPL.

### Module Layout

| Directory | Role |
|-----------|------|
| `gc/` | Core library: type system, value system, graph compile/compute, YAML parsing, JIT build infrastructure |
| `gc_app/` | Application `ComputationNode` implementations: cellular automata (`cell_aut/`), numerics (`num/`), utilities (`util/`), visual nodes (`visual/`) |
| `gc_types/` | Domain types: `Color`, `Image<Pixel>`, `IndexedPalette`, `LiveTimeSeries` |
| `gc_visual/` | Qt6 GUI: `MainWindow`, `ComputationThread`, `GraphBroker`, layout parser, parameter editors, output visualizers, video recording |
| `plot_visual/` | Time-series charts: painter backend and OpenGL 3.3 backend with incremental GPU upload |
| `sieve/` | Image metrics nodes (`i8_image_metrics`) |
| `agc_rt/`, `agc_app/`, `agc_app_rt/`, `agc_perf/` | Experimental activation graph (JIT code-gen path) |
| `3p/` | Git submodules: googletest, benchmark, magic_enum, quill, yaml-cpp |

### GUI Threading

- **Main thread**: Qt event loop, `GraphBroker` mediator, layout/widget construction
- **Computation thread** (`ComputationThread extends QThread`): runs `gc::compute()` with `stop_token`; no lock-based synchronization during execution — thread is fully stopped (`wait()`) before any state mutation
- **Video recorder thread**: FFmpeg H.264 MP4 encoding

### Conventions

- Trailing return type syntax everywhere: `auto foo() -> ReturnType`
- PIMPL with `struct Storage` (Qt widgets) or `unique_ptr<Impl>` (core types)
- `gc_app` nodes registered via `NodeRegistry` by string name; same names used in `.gc` YAML files
- `GraphBroker` uses `BindingResolver` to translate YAML binding strings (e.g. `"image_colorizer"`, `"image_size/width"`) into typed `ParameterSpec`/`IoSpec`

### Dependencies

| Library | Role |
|---------|------|
| Qt 6 (Widgets, OpenGL, OpenGLWidgets) | GUI |
| yaml-cpp | `.gc` file parsing |
| FFmpeg | Video recording |
| libpng | PNG image loading |
| quill | Async structured logging |
| magic_enum | Enum reflection |
| googletest / Google Benchmark | Tests / benchmarks |
