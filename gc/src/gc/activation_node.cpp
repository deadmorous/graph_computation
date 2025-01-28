#include "gc/activation_node.hpp"
#include "gc/algorithm.hpp"
#include "gc/algorithm_inspector.hpp"

#include "common/detail/ind.hpp"
#include "common/format.hpp"


using namespace std::string_view_literals;

namespace gc {

namespace {

using common::detail::Ind;

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

class AlgPrinter final
{
public:
    AlgPrinter(std::ostream& s,
               const alg::AlgorithmStorage& storage) noexcept:
        visitor_{ inspector_, storage, s },
        inspector_{ visitor_, false, storage }
    {}

    auto operator<<(const NodeActivationAlgorithms& algs)
        -> const AlgPrinter&
    {
        print(algs.input_bindings);
        print(algs.algorithms);
        if (algs.context != common::Zero)
            visitor_.nested("context", algs.context);

        return *this;
    }

private:
    struct Visitor;
    using Inspector = alg::AlgorithmInspector<Visitor>;

    struct Visitor final
    {
        auto operator()(alg::StepIn_Tag) noexcept
            -> void
        { ++ind_; }

        auto operator()(alg::StepOut_Tag) noexcept
            -> void
        { --ind_; }

        auto operator()(alg::id::Assign id)
            -> bool
        {
            const auto& spec = storage_(id);
            nested("assign", [&]{
                nested("to", spec.dst);
                nested("from", spec.src);
            });
            return false;
        }

        auto operator()(alg::id::Block)
            -> bool
        {
            print("block");
            return true;
        }

        auto operator()(alg::id::Do id)
            -> bool
        {
            const auto& spec = storage_(id);
            nested(
                "do",
                [&]{
                    inspector_(spec.vars);
                    nested("body", spec.body);
                    nested("while", spec.condition);
                });
            return false;
        }

        auto operator()(alg::id::For id)
            -> bool
        {
            const auto& spec = storage_(id);
            nested(
                "for",
                [&]{
                    inspector_(spec.vars);
                    nested("init", spec.init);
                    nested("condition", spec.condition);
                    nested("increment", spec.increment);
                    nested("body", spec.body);
                });
            return false;
        }

        auto operator()(alg::id::FuncInvocation id)
            -> bool
        {
            const auto& spec = storage_(id);
            auto func = storage_(spec.func);
            auto args = storage_(spec.args);
            print("func_invocation: ", func.name,
                  '(',
                  common::format_seq(args, ", ", alg_id_printer_helper),
                  ')');
            return false;
        }

        auto operator()(alg::id::HeaderFile id)
            -> bool
        {
            const auto& spec = storage_(id);
            print(
                "#include ",
                (spec.system ? '<': '"'),
                spec.name,
                (spec.system ? '>': '"'),
                " // from ", storage_(spec.lib).name);
            return false;
        }

        auto operator()(alg::id::If id)
            -> bool
        {
            const auto& spec = storage_(id);
            nested(
                "if",
                [&]{
                    inspector_(spec.vars);
                    nested("condition", spec.condition);
                    nested("then", spec.then_clause);
                    if (spec.else_clause != common::Zero)
                        nested("else", spec.else_clause);
                });
            return false;
        }

        auto operator()(alg::id::InputBinding id)
            -> bool
        {
            const auto& spec = storage_(id);
            nested(
                common::format("bind ", Prefixed{spec.var}, " <= ", spec.port),
                [&]{ inspector_(storage_(spec.var)); });
            return false;
        }

        auto operator()(alg::id::Lib id)
            -> bool
        {
            const auto& spec = storage_(id);
            print("@module ", spec.name);
            return false;
        }

        auto operator()(alg::id::OutputActivation id)
            -> bool
        {
            const auto& spec = storage_(id);
            print("activate ", Prefixed{spec.var}, " => ", spec.port);
            return false;
        }

        auto operator()(alg::id::ReturnOutputActivation id)
            -> bool
        {
            const auto& spec = storage_(id);
            nested("return output activation", spec.activation);
            return false;
        }

        auto operator()(alg::id::Statement id)
            -> bool
        {
            print("statement");
            return true;
        }

        auto operator()(alg::id::Symbol id)
            -> bool
        {
            const auto& spec = storage_(id);
            auto text = common::format("symbol ", spec.name);
            if (spec.header_file != common::Zero)
            {
                const auto& h = storage_(spec.header_file);
                text =
                    common::format(text, " defined in header '", h.name, '\'');
            }
            print(text);
            return false;
        }

        auto operator()(alg::id::Type id)
            -> bool
        {
            const auto& spec = storage_(id);
            auto text = common::format("type '", spec.name, "'");
            if (spec.header_file != common::Zero)
            {
                const auto& h = storage_(spec.header_file);
                text =
                    common::format(text, " defined in header '", h.name, '\'');
            }
            print(text);
            return false;
        }

        auto operator()(alg::id::TypeFromBinding)
            -> bool
        {
            print("type from binding");
            return true;
        }

        auto operator()(alg::id::Var id)
            -> bool
        {
            print(Prefixed(id));
            return true;
        }

        auto operator()(alg::id::Vars id)
            -> bool
        {
            print(id == common::Zero? "vars (none)": "vars");
            return true;
        }

        auto operator()(alg::id::While id)
            -> bool
        {
            const auto& spec = storage_(id);
            nested(
                "while",
                [&]{
                    inspector_(spec.vars);
                    nested("condition", spec.condition);
                    nested("body", spec.body);
                });
            return false;
        }

        template <typename T>
        auto nested(std::string_view message, const T& arg)
            ->void
        {
            print(message);
            ++ind_;
            inspector_(arg);
            --ind_;
        }

        template <typename F>
        requires requires(F f){ f(); }
        auto nested(std::string_view message, F&& f)
            -> void
        {
            print(message);
            ++ind_;
            std::forward<F>(f)();
            --ind_;
        }

        template <typename... Ts>
        auto print(Ts&&... args) const
            -> void
        {
            s_ << ind_
               << common::format(std::forward<Ts>(args)...)
               << '\n';
        }

        Inspector& inspector_;
        const gc::alg::AlgorithmStorage& storage_;
        std::ostream& s_;
        Ind ind_;
    };

    auto print(const InputBindingVec& bindings)
        -> void
    {
        visitor_.nested("input bndings", [&]{
            for (auto id : bindings)
                inspector_(id);
        });
    }

    auto print(const PortActivationAlgorithm& a)
        -> void
    {
        visitor_.print(
            "required inputs: {", common::format_seq(a.required_inputs), '}');
        visitor_.nested("activate", a.activate);
    }

    auto print(const PortActivationAlgorithmVec& algorithms)
        -> void
    {
        visitor_.nested(
            "activation algorithms",
            [&]{
                for (auto port : algorithms.index_range())
                    visitor_.nested(
                        common::format("port ", port),
                        [&]{ print(algorithms[port]); });
            });
    }


    Visitor visitor_;

    Inspector inspector_;
};

} // anonymous namespace


auto operator<<(std::ostream& s, const PrintableNodeActivationAlgorithms& a)
    -> std::ostream&
{
    const auto& [algs, storage] = a;
    // TODO
    auto printer = AlgPrinter{ s, storage };
    printer << algs;
    return s;
}

} // namespace gc
