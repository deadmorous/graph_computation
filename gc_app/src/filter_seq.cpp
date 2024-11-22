#include "gc_app/filter_seq.hpp"

#include "gc_app/types.hpp"

#include "gc/expect_n_node_args.hpp"
#include "gc/node.hpp"
#include "gc/node_port_names.hpp"


using namespace std::string_view_literals;
using namespace gc::literals;

namespace gc_app {

class FilterSeq final :
    public gc::Node
{
public:
    auto input_names() const
        -> common::ConstNameSpan override
    { return gc::node_input_names<FilterSeq>( "sequence"sv, "value"sv ); }

    auto output_names() const
        -> common::ConstNameSpan override
    { return gc::node_output_names<FilterSeq>( "indices"sv ); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 2_gc_ic);
        result[0_gc_i] = uint_vec_val({0, 1, 2, 3, 0, 1, 2, 3});
        result[1_gc_i] = uint_val(0);
    }

    auto compute_outputs(
            gc::OutputValues result,
            gc::ConstInputValues inputs,
            const std::stop_token& stoken,
            const gc::NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.size() == 2_gc_ic);
        assert(result.size() == 1_gc_oc);
        const auto& seq = uint_vec_val(inputs[0_gc_i]);
        auto value = uint_val(inputs[1_gc_i]);

        auto filtered = UintVec{};
        for (size_t i=0, n=seq.size(); i<n; ++i)
        {
            if (seq[i] == value)
                filtered.push_back(Uint(i));
            if (stoken.stop_requested())
                return false;
        }

        result.front() = uint_vec_val(std::move(filtered));
        return true;
    }
};

auto make_filter_seq(gc::ConstValueSpan args)
-> std::shared_ptr<gc::Node>
{
    gc::expect_no_node_args("FilterSeq", args);
    return std::make_shared<FilterSeq>();
}

} // namespace gc_app
