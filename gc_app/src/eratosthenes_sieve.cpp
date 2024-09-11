#include "eratosthenes_sieve.hpp"

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

}

auto EratosthenesSieve::generate(Uint limit)
    -> ConstUintSpan
{
    if (state_.size() < limit)
        state_ = sieve(limit);

    return {state_.data(), limit};
}


#if 0
struct _{_(){
    PrimeInt Ns[] = {
        1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000, 1'000'000'000 };
    for (auto N : Ns)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        auto s = EratosthenesSieve::sieve(N);
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
