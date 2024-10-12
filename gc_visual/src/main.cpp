#include "gc_visual/mainwindow.hpp"

#include "gc_app/source_param.hpp"
#include "gc_app/node_registry.hpp"

#include "gc/graph_computation.hpp"

#include <QApplication>


using namespace std::string_view_literals;

auto run(int argc, char *argv[])
    -> int
{
    QApplication a(argc, argv);

    auto obj_reg =
        gc_app::node_registry();

    auto img_size = obj_reg.at("source_param")(gc::ValueVec{1});
    auto img_size_w = obj_reg.at("source_param")(gc::ValueVec{1});
    auto img_size_h = obj_reg.at("source_param")(gc::ValueVec{1});

    auto pw = obj_reg.at("project")({});
    auto ph = obj_reg.at("project")({});

    auto seq_size = obj_reg.at("multiply")({});
    auto sieve = obj_reg.at("eratosthenes_sieve")({});
    auto view = obj_reg.at("rect_view")({});

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
