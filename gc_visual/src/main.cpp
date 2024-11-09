#include "gc_visual/mainwindow.hpp"

#include <QApplication>

#include <iostream>


using namespace std::string_view_literals;

constexpr auto* example_config_text = R"(
graph:
  nodes:
    - name: img_size
      type: uint_size
    - name: seq_size
      type: multiply
    - name: sieve
      type: eratosthenes_sieve
    - name: view
      type: rect_view
    - name: filter
      type: filter_seq
  edges:
    - [seq_size.product,    sieve.count]
    - [img_size,            view.size]
    - [sieve.sequence,      view.sequence]
    - [sieve.sequence,      filter.sequence]
  inputs:
    - name: img_width
      type: U32
      value: 600
      destinations: [img_size.width, seq_size.lhs]
    - name: img_height
      type: U32
      value: 500
      destinations: [img_size.height, seq_size.rhs]
    - name: palette
      type: IndexedPalette
      value:
        color_map:
          - 0xffffffff
          - 0xff5953ff
          - 0xff8adabf
          - 0xffffc7f9
          - 0xffffff7f
          - 0xffd40004
          - 0xff00aa00
        overflow_color: 0xff000000
      destinations: [view.palette]
    - name: filter
      type: U32
      value: 0
      destinations: [filter.value]

layout:
  type: horizontal_layout
  items:
    - type: vertical_layout
      items:
      - type: horizontal_layout
        items:
        - type: spin
          bind: img_width
          range: [1, 5000]
        - type: spin
          bind: img_height
          range: [1, 2000]
      - type: color
        bind: palette/overflow_color
      - type: vector
        bind: palette/color_map
      - type: spin
        bind: filter
        range: [0, 100]
      - type: stretch
    - type: vertical_layout
      items:
      - type: image
        bind: view
      - type: text
        bind: filter.indices
)";

auto run(int argc, char *argv[])
    -> int
{
    QApplication a(argc, argv);

    MainWindow w({
        .type = gc_visual::ConfigSpecificationType::Content,
        .spec = example_config_text });
    w.show();

    return a.exec();
}

int main(int argc, char *argv[])
{
    try
    {
        return run(argc, argv);
    }
    catch(std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
