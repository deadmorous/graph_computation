#include "gc_visual/mainwindow.hpp"

#include "gc_app/eratosthenes_sieve.hpp"
#include "gc_app/image.hpp"
#include "gc_app/multiply.hpp"
#include "gc_app/project.hpp"
#include "gc_app/rect_view.hpp"
#include "gc_app/source_param.hpp"
// #include "gc_app/test_sequence.hpp"

#include <QApplication>


using namespace std::string_view_literals;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto img_size = gc_app::make_source_param();
    auto img_size_w = gc_app::make_source_param();
    auto img_size_h = gc_app::make_source_param();

    auto pw = gc_app::make_project();
    auto ph = gc_app::make_project();

    auto seq_size = gc_app::make_multiply();
    auto sieve = gc_app::make_eratosthenes_sieve();
    auto view = gc_app::make_rect_view();

    gc_app::InputParameters::get(img_size.get())
        ->set_inputs(gc::ValueVec{ gc_app::UintSize(500, 500) });

    gc_app::InputParameters::get(img_size_w.get())
        ->set_inputs(gc::ValueVec{ gc::ValuePath{} / "width"sv });

    gc_app::InputParameters::get(img_size_h.get())
        ->set_inputs(gc::ValueVec{ gc::ValuePath{} / "height"sv });

    using NodeVec = std::vector<gc::NodePtr>;

    using EE = gc::EdgeEnd;
    auto g = gc::Graph{
        .nodes = { img_size,
                   img_size_w, img_size_h, pw, ph, seq_size,
                   sieve, view },
        .edges = {{EE{img_size.get(), 0}, EE{pw.get(), 0}},
                  {EE{img_size_w.get(), 0}, EE{pw.get(), 1}},

                  {EE{img_size.get(), 0}, EE{ph.get(), 0}},
                  {EE{img_size_h.get(), 0}, EE{ph.get(), 1}},

                  {EE{pw.get(), 0}, EE{seq_size.get(), 0}},
                  {EE{ph.get(), 0}, EE{seq_size.get(), 1}},

                  {EE{seq_size.get(), 0}, EE{sieve.get(), 0}},
                  {EE{img_size.get(), 0}, EE{view.get(), 0}},
                  {EE{sieve.get(), 0}, EE{view.get(), 1}}
        }
    };

    auto instr = compile(g);
    auto result = gc::ComputationResult{};
    compute(result, g, instr.get());
    const auto& image = group(result.outputs,7)[0].as<gc_app::Image>();

    MainWindow w(image);
    w.show();

    return a.exec();
}
