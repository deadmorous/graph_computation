#include "gc_app/eratosthenes_sieve.hpp"

#include "gc_app/types.hpp"

#include "gc/node_port_names.hpp"

// #include <chrono>


using namespace std::string_view_literals;

namespace gc_app {
namespace {

auto sieve(Uint limit)
    -> UintVec
{
    auto result = UintVec(limit, 0);
    auto prime = Uint{2};
    while (prime+1 < limit)
    {
        for (auto n=prime*2; n<limit; n+=prime)
            ++result[n];
        for (++prime; prime<limit; ++prime)
            if (result[prime] == 0)
                break;
    }
    return result;
}

} // anonymous namespace


class EratosthenesSieve final :
    public gc::Node
{
public:
    auto input_names() const
        -> common::ConstNameSpan
    { return gc::node_input_names<EratosthenesSieve>( "count"sv ); }

    auto output_names() const
        -> common::ConstNameSpan
    { return gc::node_output_names<EratosthenesSieve>( "sequence"sv ); }

    auto default_inputs(gc::ValueSpan result) const
        -> void
    {
        assert(result.size() == 1);
        result[0] = uint_val(1000);
    }

    auto compute_outputs(
            gc::ValueSpan result,
            gc::ConstValueSpan inputs) const
        -> void
    {
        assert(inputs.size() == 1);
        assert(result.size() == 1);
        auto count = uint_val(inputs[0]);
        result[0] = uint_vec_val(sieve(count));
    }
};

#if 0
struct _{_(){
    Uint Ns[] = {
        1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000, 1'000'000'000 };
    for (auto N : Ns)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        auto s = sieve(N);
        auto t2 = std::chrono::high_resolution_clock::now();
        auto dt = std::chrono::nanoseconds{ t2 - t1 };
        // std::cout
        //     << "Time to find primes in the range up to " << N
        //     << ": " << dt.count() / 1e9 << " s" << std::endl;
        std::cout
            << N
            << '\t' << dt.count() / 1e9 << std::endl;
    }
    // for (size_t i=0, n=s.size(); i<n; ++i)
    //     std::cout
    //         << i << '\t' << s[i]
    //         << '\t' << (s[i] == 0 ? '*': ' ')
    //         << std::endl;
}}__;
#endif // 0

auto make_eratosthenes_sieve()
    -> std::shared_ptr<gc::Node>
{ return std::make_shared<EratosthenesSieve>(); }

} // namespace gc_app
