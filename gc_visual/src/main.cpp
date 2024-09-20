#include "gc_visual/mainwindow.hpp"

#include "gc_visual/simple_presentation.hpp"
#include "gc_visual/spiral_presentation.hpp"

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

    auto img_size = std::make_shared<gc_app::SourceParam>();
    auto img_size_w = std::make_shared<gc_app::SourceParam>();
    auto img_size_h = std::make_shared<gc_app::SourceParam>();

    auto pw = std::make_shared<gc_app::Project>();
    auto ph = std::make_shared<gc_app::Project>();

    auto seq_size = std::make_shared<gc_app::Multiply>();
    auto sieve = std::make_shared<gc_app::EratosthenesSieve>();
    auto view = std::make_shared<gc_app::RectView>();

    img_size->set_inputs(gc::ValueVec{ gc_app::UintSize(500, 500) });
    img_size_w->set_inputs(gc::ValueVec{ gc::ValuePath{} / "width"sv });
    img_size_h->set_inputs(gc::ValueVec{ gc::ValuePath{} / "height"sv });

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
    // TODO: copy to QImage

#if 0 // TODO
    // TestSequence generator;
    EratosthenesSieve generator;

    SimplePresentation presentation;
    //SpiralPresentation presentation;

    MainWindow w(generator, presentation);
    w.show();
#endif // 0
    return a.exec();
}
