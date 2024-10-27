#include "gc_app/filter_seq.hpp"

#include "gc_app/types.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node.hpp"
#include "gc/node_port_names.hpp"


using namespace std::string_view_literals;

namespace gc_app {

class FilterSeq final :
    public gc::Node
{
public:
    auto input_names() const
        -> common::ConstNameSpan
    { return gc::node_input_names<FilterSeq>( "sequence"sv, "value"sv ); }

    auto output_names() const
        -> common::ConstNameSpan
    { return gc::node_output_names<FilterSeq>( "indices"sv ); }

    auto default_inputs(gc::ValueSpan result) const
        -> void
    {
        assert(result.size() == 2);
        result[0] = uint_vec_val({0, 1, 2, 3, 0, 1, 2, 3});
        result[1] = uint_val(0);
    }

    auto compute_outputs(
            gc::ValueSpan result,
            gc::ConstValueSpan inputs) const
        -> void
    {
        assert(inputs.size() == 2);
        assert(result.size() == 1);
        const auto& seq = uint_vec_val(inputs[0]);
        auto value = uint_val(inputs[1]);

        auto filtered = UintVec{};
        for (size_t i=0, n=seq.size(); i<n; ++i)
            if (seq[i] == value)
                filtered.push_back(Uint(i));

        result[0] = uint_vec_val(std::move(filtered));
    }
};

auto make_filter_seq(gc::ConstValueSpan args)
-> std::shared_ptr<gc::Node>
{
    gc::expect_no_node_args("FilterSeq", args);
    return std::make_shared<FilterSeq>();
}

} // namespace gc_app
