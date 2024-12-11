#include "gc/activation_node.hpp"
#include "gc/algorithm.hpp"

#include "common/format.hpp"


namespace gc {

namespace {

class AlgPrinterHelper
{
public:
    explicit AlgPrinterHelper(const alg::AlgorithmStorage& storage) noexcept:
        storage_{ storage }
    {}

    auto operator()(std::ostream& s, alg::id::InputBinding b_id) const
        -> void
    {
        const auto& binding = storage_(b_id);
        s << binding.port << " => " << "var_" << binding.var;
    }

    auto operator()(std::ostream& s, const PortActivationAlgorithm& a) const
        -> void
    {
        // s << "Required inputs: " << a.required_inputs;
        s << "TODO\n";
    }

    template <typename T>
    auto operator()(std::ostream& s, const T& value) const
        -> void
    { s << value; }

private:

    const alg::AlgorithmStorage& storage_;
};


class AlgPrinter
{
public:
    AlgPrinter(std::ostream& s,
               const alg::AlgorithmStorage& storage) noexcept:
        s_{ s },
        helper_{ storage }
    {}

    template <typename Seq>
    auto seq(const Seq& seq, std::string_view delim = ", ") const
        -> std::string
    { return common::format_seq(seq, delim, helper_); }

    template <typename T>
    auto operator<<(const T& value) const
        -> const AlgPrinter&
    {
        helper_(s_, value);
        return *this;
    }

    auto operator<<(std::ostream& (*manip)(std::ostream&)) const
        -> const AlgPrinter&
    {
        s_ << manip;
        return *this;
    }

private:
    std::ostream& s_;
    AlgPrinterHelper helper_;
};

} // anonymous namespace


auto operator<<(std::ostream& s, const PrintableNodeActivationAlgorithms& a)
    -> std::ostream&
{
    auto printer = AlgPrinter{ s, a.alg_storage };
    const auto& alg = a.algs;

    printer
        << "Input bindings: [" << printer.seq(alg.input_bindings) << ']'
        << std::endl;

    for (auto port : alg.algorithms.index_range())
    {
        printer << "Activation algorithm for port "<< port << std::endl;
        printer << alg.algorithms[port];
    }

    return s;
}

} // namespace gc
