# graph_computation

A C++20 dataflow computation engine built around directed acyclic graphs (DAGs).
You define a computation as a graph of typed nodes connected by edges, supply
initial inputs, and the engine executes each node in topological order,
propagating outputs downstream. The project ships with a Qt-based GUI
application (`gc_visual`) for interactive exploration, and a headless CLI tool
(`gc_cli`).

**Author:** Stepan Orlov, MPK Software (St. Petersburg, Russia), 2024-2026.

## How it works

A user writes a `.gc` file (YAML format) describing:

- **graph** &mdash; the nodes, edges between them, and initial input values.
- **evolution** (optional) &mdash; feedback loops that drive step-by-step
  simulation (e.g. cellular automaton generations).
- **layout** &mdash; the Qt UI: which parameter editors and output visualizers
  to display and how they bind to graph elements.

The `.gc` file is loaded by the `gc_visual` application (via File > Open or as
a command-line argument). The engine parses the YAML, compiles the graph into a
level-sorted execution plan, runs the computation on a background thread, and
builds the UI from the layout section.

## What you can do

- Adjust parameters in real time via spin boxes, color pickers, file pickers,
  vector editors, and more.
- Watch output images update reactively after each re-computation.
- Step through evolution (cellular automaton generations) frame by frame or in
  batches.
- Reset initial conditions at any time.
- Record output to video (MP4 via FFmpeg).
- Stop a long-running computation early.

## Built-in node types

### Cellular automata (`cell_aut`)

| Node | Description |
|---|---|
| `cell2d` | 2D cellular automaton with configurable multi-state rules and torus/rect topology |
| `life` | Conway's Game of Life variant |
| `random_image` | Randomized initial state generator |
| `image_loader` | Load a PNG as initial state |
| `image_colorizer` | Map an `I8Image` to a `ColorImage` via an indexed palette |
| `rule_reader`, `gen_rule_reader` | Read rule files (`.rul`, `.gen`) from disk |
| `generate_cmap`, `gen_cmap_reader` | Colormap generation / reading |
| `offset_image` | Spatial offset transform |

### Numerical (`num`)

| Node | Description |
|---|---|
| `eratosthenes_sieve` | Prime number sieve producing a sequence |
| `waring`, `waring_parallel` | Waring's problem solver |
| `filter_seq` | Filter a sequence by divisibility |
| `multiply` | Multiply two U32 values |
| `test_sequence` | Test sequence generator |

### Utility (`util`)

| Node | Description |
|---|---|
| `project` | Extract a named sub-value from a complex type |
| `merge` | Merge multiple values into a composite type |
| `uint_size` | Construct a `UintSize` from width and height |

### Visual (`visual`)

| Node | Description |
|---|---|
| `rect_view` | Render a number sequence as a rectangular bitmap |
| `spiral_view` | Render a number sequence as a spiral bitmap |
| `image_loader` | Load a PNG into an `I8Image` |

### Sieve (image metrics)

| Node | Description |
|---|---|
| `i8_image_metrics` | Compute state histogram, edge histogram, and plateau average size over a cellular automaton frame |

## UI widget types

### Parameter editors

| Widget | Description |
|---|---|
| `spin` | Integer or float spin box with configurable range |
| `color` | ARGB color picker |
| `vector` | Editable list of colors or other values |
| `file` | File picker with filter string |
| `cell2d_rules` | Dedicated editor for `Cell2dRules` structures |
| `cell2d_gen_rules` / `cell2d_gen_cmap` | Editors for generated rule/colormap types |
| `flags` | Checkbox set for enum flags |
| `list` | Generic list editor |
| `invalidate` | Button that forces re-computation (used for "reset" in evolution) |
| `evolution` | Step/run controller for feedback-driven simulation |

### Output visualizers

| Widget | Description |
|---|---|
| `image` | Displays a `ColorImage`; supports optional blend mode |
| `image_metrics` | Live scrolling time-series charts (histogram, edge histogram, plateau size) |
| `text` | Displays a value as text |

## Example `.gc` file

```yaml
graph:
  nodes:
    - type: random_image
      name: initial_image
    - name: cell2d
      type: cell2d
    - name: image_colorizer
      type: image_colorizer

  edges:
    - [initial_image.image, cell2d.input_state]
    - [cell2d.output_state, image_colorizer.input_image]

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
      destinations: [image_colorizer.palette]

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
      bind: image_colorizer
```

### YAML reference

**`graph.nodes[]`** &mdash; each entry has `name` (unique identifier), `type`
(registered node type name), and optional `init` (compile-time constructor
arguments).

**`graph.edges[]`** &mdash; each entry is `[from_node.port, to_node.port]`.
If a node has a single input or output, the port name can be omitted.

**`graph.inputs[]`** &mdash; named external inputs with `type`, `value`, and
`destinations` (list of `node.port` targets).

**`evolution.feedback[]`** &mdash; loops a `source` node output back to one or
more `sink` node inputs each step.

**`layout`** &mdash; a recursive tree of containers (`horizontal_layout`,
`vertical_layout`, `stretch`) and leaf widgets. Widgets use `bind` to reference
a named input or `node.port` output. Path navigation uses `/`
(e.g. `palette/overflow_color`).

## Building

Prerequisites: C++20 compiler, Qt 6, CMake, yaml-cpp. Optional: FFmpeg
(for video recording), quill (for logging).

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=<path-to-qt6>
cmake --build build -j$(nproc)
```

## Running

```bash
# GUI
./build/gc_visual/gc_visual examples/cell_aut/cell2d.gc

# Headless
./build/gc_cli/gc_cli examples/cell_aut/cell2d.gc
```
