Activation graph TODO
=====================

## 09.08.2025

+ Check that input types specified by user in the `source_types` argument
  to `gc::generate_source` are consistent with what node defines: if
  "from binding", a type is expected; otherwise, type must be either
  unspecified or coincide with what node defines.
+ Check (maybe optionally?) that all external inputs are provided
- An API implementing graph reflection and context manipulation:
  - List ultimate external inputs and their types
  + Map external inputs to context members
  + Map useful node states (e.g., canvas) to context members
  + Create context instance
  - Set external input into context variable, perhaps converted to gc::Value
  - Read context variable, perhaps converted to gc::Value
  + Pass context to entry point
- Parse graph specification from YAML
- GUI integration
  - Printer node: output stream specification
  - Interrupting using atomic valves and additional entry points
  - Image/other output
    Node that periodically outputs an image (e.g. when each N-th pixel is set)
    to a copy available to its consumer, running in other threads; the access
    to the copy is locked with a mutex.
- Bigger building blocks
