#include "gc_visual/mainwindow.hpp"

#include <QApplication>

#include <iostream>


using namespace std::string_view_literals;

constexpr auto* example_config_text = R"(
graph:
  nodes:
    - name:  img_size
      type:  source_param
      init:
        - type: UintSize
          value:
            width: 600
            height: 500

    - name:  img_size_w
      type:  source_param
      init:
        - type: ValuePath
          value: /width

    - name:  img_size_h
      type:  source_param
      init:
        - type: ValuePath
          value: /height

    - name: palette
      type: source_param
      init:
        - type: IndexedPalette
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

    - name: filter_value
      type: source_param
      init:
        - type: U32
          value: 0

    - name: pw
      type: project

    - name: ph
      type: project

    - name: seq_size
      type: multiply

    - name: sieve
      type: eratosthenes_sieve

    - name: view
      type: rect_view

    - name: filter
      type: filter_seq

  edges:
    - [img_size.0,          pw.value]
    - [img_size_w.out_0,    pw.path]
    - [img_size,            ph.value]
    - [img_size_h,          ph.path]
    - [pw.projection,       seq_size.lhs]
    - [ph.projection,       seq_size.rhs]
    - [seq_size.product,    sieve.count]
    - [img_size,            view.size]
    - [sieve.sequence,      view.sequence]
    - [palette,             view.palette]
    - [filter_value,        filter.value]
    - [sieve.sequence,      filter.sequence]

layout:
  type: horizontal_layout
  items:
    - type: vertical_layout
      items:
      - type: horizontal_layout
        items:
        - type: spin
          bind:
            node: img_size
            index: 0
            path: /width
          range: [1, 5000]
        - type: spin
          bind:
            node: img_size
            index: 0
            path: /height
          range: [1, 2000]
      - type: color
        bind:
          node: palette
          path: /overflow_color
      - type: vector
        bind:
          node: palette
          path: /color_map
      - type: spin
        bind:
          node: filter_value
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
