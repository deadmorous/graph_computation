#include "gc_app/waring.hpp"

#include "gc_app/multi_index.hpp"

#include "gc_app/types.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node.hpp"
#include "gc/node_port_names.hpp"

#include <cmath>
#include <numeric>

using namespace std::string_view_literals;

namespace gc_app {
namespace {

auto waring(Uint limit, Uint s, Uint k)
-> UintVec
{
    auto result = UintVec(limit, 0);

    auto tlim = static_cast<Uint>(pow(limit, 1./k)) + 1;
    auto n = UintVec(tlim);
    std::iota(n.begin(), n.end(), 0);

    for (auto& nk : n)
    {
        auto n1 = nk;
        for (Uint d=1; d<k; ++d)
            nk *= n1;
    }

    auto index = std::vector<Uint>(s, 0);

    do
    {
        Uint sum = 0;
        for (auto i : index)
            sum += n[i];
        if (sum < limit)
            ++result[sum];
    }
    while(inc_multi_index_mono(index, tlim));

    return result;
}

} // anonymous namespace

class Waring final :
    public gc::Node
{
public:
    auto input_names() const
        -> common::ConstNameSpan
    { return gc::node_input_names<Waring>( "count"sv, "s"sv, "k"sv ); }

    auto output_names() const
        -> common::ConstNameSpan
    { return gc::node_output_names<Waring>( "sequence"sv ); }

    auto default_inputs(gc::ValueSpan result) const
        -> void
    {
        assert(result.size() == 3);
        result[0] = uint_val(1000);
        result[0] = uint_val(2);
        result[0] = uint_val(2);
    }

    auto compute_outputs(
            gc::ValueSpan result,
            gc::ConstValueSpan inputs) const
        -> void
    {
        assert(inputs.size() == 3);
        assert(result.size() == 1);
        auto count = uint_val(inputs[0]);
        auto s = uint_val(inputs[1]);
        auto k = uint_val(inputs[2]);
        result[0] = uint_vec_val(waring(count, s, k));
    }
};

auto make_waring(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::Node>
{
    gc::expect_no_node_args("Waring", args);
    return std::make_shared<Waring>();
}

} // namespace gc_app
