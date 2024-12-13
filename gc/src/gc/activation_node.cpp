#include "gc/activation_node.hpp"
#include "gc/algorithm.hpp"

#include "common/format.hpp"


using namespace std::string_view_literals;

namespace gc {

namespace {

// --- Indentation

GCLIB_STRONG_TYPE(Ind, uint16_t, 0, common::StrongCountFeatures);

constexpr size_t tab_size = 2;

auto operator<<(std::ostream& s, Ind ind)
    -> std::ostream&
{
    for (Ind::Weak i=0, n=tab_size*ind.v; i<n; ++i)
        s << ' ';
    return s;
}

auto next(Ind ind)
    -> Ind
{ return ind + Ind{1u}; }


// ---

constexpr auto none = "<none>"sv;


template <typename>
constexpr std::string_view prefix_for = {};

#define GCLIB_IMPL_PREFIX_FOR(Type, prefix)                                 \
    template<>                                                              \
    constexpr std::string_view prefix_for<Type> = #prefix

GCLIB_IMPL_PREFIX_FOR(alg::id::Block, block_);
GCLIB_IMPL_PREFIX_FOR(alg::id::Do, do_);
GCLIB_IMPL_PREFIX_FOR(alg::id::For, for_);
GCLIB_IMPL_PREFIX_FOR(alg::id::FuncInvocation, func_invocation_);
GCLIB_IMPL_PREFIX_FOR(alg::id::HeaderFile, header_file_);
GCLIB_IMPL_PREFIX_FOR(alg::id::If, if_);
GCLIB_IMPL_PREFIX_FOR(alg::id::InputBinding, input_binding_);
GCLIB_IMPL_PREFIX_FOR(alg::id::Lib, lib_);
GCLIB_IMPL_PREFIX_FOR(alg::id::OutputActivation, output_activation_);
GCLIB_IMPL_PREFIX_FOR(alg::id::Statement, statement_);
GCLIB_IMPL_PREFIX_FOR(alg::id::Symbol, symbol_);
GCLIB_IMPL_PREFIX_FOR(alg::id::Type, type_);
GCLIB_IMPL_PREFIX_FOR(alg::id::Var, var_);
GCLIB_IMPL_PREFIX_FOR(alg::id::Vars, vars_);
GCLIB_IMPL_PREFIX_FOR(alg::id::While, while_);



struct AlgIdPrinterHelper final
{
    template<typename Id>
    auto operator()(std::ostream& s, Id id) const
        -> void
    {
        s << prefix_for<Id>;
        if (id == common::Zero)
            s << none;
        else
            s << id;
    }
};

constexpr auto alg_id_printer_helper = AlgIdPrinterHelper{};

template <typename Id>
struct Prefixed final
{
    Id id;
};

template <typename Id>
auto operator<<(std::ostream& s, Prefixed<Id> prefixed)
    -> std::ostream&
{
    alg_id_printer_helper(s, prefixed.id);
    return s;
}

class AlgPrinterHelper final
{
public:
    explicit AlgPrinterHelper(const alg::AlgorithmStorage& storage) noexcept:
        storage_{ storage }
    {}

    auto operator()(std::ostream& s, alg::id::Block id, Ind ind = {}) const
        -> void
    {
        const auto& spec = storage_(id);
        s << ind << "block\n";
        auto next_ind = next(ind);
        (*this)(s, spec.vars, next_ind);
        for (auto statement_id : spec.statements)
            (*this)(s, statement_id, next_ind);
    }

    auto operator()(std::ostream& s, alg::id::Do id, Ind ind = {}) const
        -> void
    {
        const auto& spec = storage_(id);
        s << ind << "do\n";
        auto next_ind = next(ind);
        auto next2_ind = next(next_ind);
        (*this)(s, spec.vars, next_ind);
        s << next_ind << "body\n";
        (*this)(s, spec.body, next2_ind);
        s << "while\n";
        s << next_ind << "condition\n";
        (*this)(s, spec.condition, next2_ind);
    }

    auto operator()(std::ostream& s, alg::id::For id, Ind ind = {}) const
        -> void
    {
        const auto& spec = storage_(id);
        s << "for\n";
        auto next_ind = next(ind);
        auto next2_ind = next(next_ind);
        (*this)(s, spec.vars, next_ind);
        s << next_ind << "init\n";
        (*this)(s, spec.init, next2_ind);
        s << next_ind << "condition\n";
        (*this)(s, spec.condition, next2_ind);
        s << next_ind << "increment\n";
        (*this)(s, spec.increment, next2_ind);
        s << next_ind << "body\n";
        (*this)(s, spec.body, next2_ind);
    }

    auto operator()(std::ostream& s,
                    alg::id::FuncInvocation id,
                    Ind ind = {}) const
        -> void
    {
        const auto& spec = storage_(id);
        auto func = storage_(spec.func);
        auto args = storage_(spec.args);
        s << ind;
        if (spec.result != common::Zero)
            s << Prefixed{spec.result} << " <- ";
        s << func.name << '('
          << common::format_seq(args, ", ", alg_id_printer_helper)
          << ")\n";
    }

    auto operator()(std::ostream& s,
                    alg::id::HeaderFile id,
                    Ind ind = {}) const
        -> void
    {
        const auto& spec = storage_(id);
        s << ind
          << "#include "
          << (spec.system ? '<': '"')
          << spec.name
          << (spec.system ? '>': '"')
          << " from " << storage_(spec.lib).name;
    }

    auto operator()(std::ostream& s, alg::id::If id, Ind ind = {}) const
        -> void
    {
        const auto& spec = storage_(id);
        s << ind << "if\n";
        auto next_ind = next(ind);
        auto next2_ind = next(next_ind);
        (*this)(s, spec.vars, next_ind);
        s << next_ind << "condition\n";
        (*this)(s, spec.condition);
        s << next_ind << "then\n";
        (*this)(s, spec.then_clause, next2_ind);
        if (spec.else_clause != common::Zero)
        {
            s << next_ind << "else\n";
            (*this)(s, spec.else_clause, next2_ind);
        }
    }

    auto operator()(std::ostream& s,
                    alg::id::InputBinding id,
                    Ind ind = {}) const
        -> void
    {
        const auto& spec = storage_(id);
        s << ind
          << Prefixed{spec.var}
          << " <= "
          << spec.port
          << '\n';
        std::visit(
            [&](auto id){ (*this)(s, spec.var, next(ind)); },
            storage_(spec.var));
    }

    auto operator()(std::ostream& s, alg::id::Lib id, Ind ind = {}) const
        -> void
    {
        const auto& spec = storage_(id);
        s << ind << "@module " << spec.name;
    }

    auto operator()(std::ostream& s,
                    alg::id::OutputActivation id,
                    Ind ind = {}) const
        -> void
    {
        const auto& spec = storage_(id);
        s << ind << Prefixed{spec.var} << " => " << spec.port;
    }

    auto operator()(std::ostream& s,
                    alg::id::Statement id,
                    Ind ind = {}) const
        -> void
    {
        if (id == common::Zero)
        {
            s << ind;
            alg_id_printer_helper(s, id);
            s << '\n';
            return;
        }
        const auto& spec = storage_(id);
        const auto& self = *this;
        std::visit(
            [&]<typename St>(const St& st)
            { self(s, st, ind); },
            spec);
    }

    auto operator()(std::ostream& s, alg::id::Symbol id, Ind ind = {}) const
        -> void
    {
        const auto& spec = storage_(id);
        s << ind
          << "symbol "
          << spec.name
          << " defined in header "
          << storage_(spec.header_file).name;
    }

    auto operator()(std::ostream& s, alg::id::Type id, Ind ind = {}) const
        -> void
    {
        const auto& spec = storage_(id);
        s << ind
          << "type "
          << spec.name
          << " defined in header "
          << storage_(spec.header_file).name
          << '\n';
    }

    auto operator()(std::ostream& s, alg::id::Var id, Ind ind = {}) const
        -> void
    {
        const auto& spec = storage_(id);
        s << ind << Prefixed(id) << '\n';
        std::visit(
            [&](const auto& init_id)
            { (*this)(s, init_id, next(ind)); },
            spec);
    }

    auto operator()(std::ostream& s, alg::id::Vars id, Ind ind = {}) const
        -> void
    {
        s << ind << "vars";
        auto next_ind = next(ind);
        if (id == common::Zero)
        {
            s << " (none)\n";
            return;
        }
        s << '\n';
        const auto& spec = storage_(id);
        for (auto var_id : spec)
            (*this)(s, var_id, next_ind);
    }

    auto operator()(std::ostream& s, alg::id::While id, Ind ind = {}) const
        -> void
    {
        const auto& spec = storage_(id);
        s << ind << "while\n";
        auto next_ind = next(ind);
        auto next2_ind = next(next_ind);
        (*this)(s, spec.vars, next_ind);
        s << next_ind << "condition\n";
        (*this)(s, spec.condition, next2_ind);
        s << next_ind << "body\n";
        (*this)(s, spec.body, next2_ind);
    }

    auto operator()(std::ostream& s,
                    const PortActivationAlgorithm& a,
                    Ind ind = {}) const
        -> void
    {
        s << ind << "Required inputs: {"
          << common::format_seq(a.required_inputs)
          << "}\n";
        s << ind << "context\n";
        (*this)(s, a.context, next(ind));
        s << ind << "activate\n";
        (*this)(s, a.activate, next(ind));
    }

    auto operator()(std::ostream& s,
                    const InputBindingVec& bindings,
                    Ind ind = {}) const
        -> void
    {
        s << ind << "Input bindings\n";
        auto next_ind = next(ind);
        for (auto id : bindings)
            (*this)(s, id, next_ind);
    }

    auto operator()(std::ostream& s,
                    const PortActivationAlgorithmVec& algorithms,
                    Ind ind = {}) const
        -> void
    {
        s << ind << "activation algorithms\n";
        auto next_ind = next(ind);
        auto next2_ind = next(next_ind);
        for (auto port : algorithms.index_range())
        {
            s << next_ind << "port " << port << '\n';
            (*this)(s, algorithms[port], next2_ind);
        }
    }


    // template <typename T>
    // auto operator()(std::ostream& s, const T& value) const
    //     -> void
    // { s << value; }

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

    // template <typename Seq>
    // auto seq(const Seq& seq, std::string_view delim = ", ") const
    //     -> std::string
    // { return common::format_seq(seq, delim, helper_); }

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

    printer << alg.input_bindings;
    printer << alg.algorithms;

    // for (auto port : alg.algorithms.index_range())
    // {
    //     printer << "Activation algorithm for port "<< port << std::endl;
    //     printer << alg.algorithms[port];
    // }

    return s;
}

} // namespace gc
