/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "common/throw.hpp"

#include <cassert>
#include <concepts>
#include <span>
#include <type_traits>
#include <variant>
#include <vector>


namespace gc::detail {

enum class SeqType
{
    AllInOrder,
    Every
};

struct BeginSeqMark
{
    SeqType type;
};

struct EndSeqMark {};

template <typename T>
using NestedSequenceItem = std::variant<BeginSeqMark, EndSeqMark, T>;

template <typename T>
using NestedSequence = std::vector<NestedSequenceItem<T>>;

template <typename T>
using NestedSequenceView = std::span<const NestedSequenceItem<T>>;

template <typename P, typename T>
concept NestedSequenceTest =
    requires(P p, T item)
    {
        requires std::is_copy_assignable_v<P>;
        requires std::is_copy_constructible_v<P>;
        { p(item) } -> std::same_as<bool>;
    };

template <typename T, NestedSequenceTest<T> P>
auto test_sequence_impl(NestedSequenceView<T> avail, P& p)
    -> std::pair<size_t, bool>
{
    if(avail.size() <= 1)
        common::throw_<std::invalid_argument>(
            "test_sequence() expects an input sequence "
            "of at least two elements");
    if(!std::holds_alternative<BeginSeqMark>(avail.front()))
        common::throw_<std::invalid_argument>(
            "test_sequence() expects an input sequence "
            "starting with a BeginSeqMark");
    auto type = std::get<BeginSeqMark>(avail.front()).type;
    size_t i = 1;
    size_t n = avail.size();
    auto p_backup = p;
    while (i < n)
    {
        const auto& item = avail[i];
        if (std::holds_alternative<BeginSeqMark>(item))
        {
            auto [nsub, ok] = test_sequence_impl(avail.subspan(i), p);
            if (!ok)
                return { i + nsub, false };
            i += nsub;
        }
        else if (std::holds_alternative<EndSeqMark>(item))
            return { i+1, true };
        else
        {
            assert(std::holds_alternative<T>(item));
            const auto& v = std::get<T>(item);
            ++i;
            if (!p(v))
                return { i, false };
        }
        if (i < n && type == SeqType::Every)
            p = p_backup;
    }

    common::throw_<std::invalid_argument>(
        "test_sequence() expects an input sequence "
        "ending with an EndSeqMark");
}

template <typename T, NestedSequenceTest<T> P>
auto test_sequence(NestedSequenceView<T> seq, P p, P& final_state)
    -> bool
{
    final_state = p;
    while (true)
    {
        auto [n, ok] = test_sequence_impl(seq, final_state);
        if (!ok)
        {
            final_state = p;
            return false;
        }
        if (n == seq.size())
            return true;
        seq = seq.subspan(n);
    }
}

// TODO
//      There actually may be many final states, depending on
//      which path is chosen in subsequences of type `SeqType::Every`.
//
//      Also, we lack subsequences of type `SeqType::Optional`; having these
//      would increase the number of possible final states, too.
//
//      At the moment, `final_state` corresponds to the last item of each
//      non-empty `SeqType::Every` subsequence.
template <typename T, NestedSequenceTest<T> P>
auto test_sequence(const NestedSequence<T>& seq, P p, P& final_state)
    -> bool
{
    auto view = NestedSequenceView<T>{ seq };
    return test_sequence(view, std::move(p), final_state);
}

template <typename T, NestedSequenceTest<T> P>
auto test_sequence(NestedSequenceView<T> seq, P p)
    -> bool
{
    auto final_state = p;
    return test_sequence(seq, std::move(p), final_state);
}

template <typename T, NestedSequenceTest<T> P>
auto test_sequence(const NestedSequence<T>& seq, P p)
    -> bool
{
    auto final_state = p;
    return test_sequence(seq, std::move(p), final_state);
}


template <typename T>
class ScopedNestedSequence
{
public:
    using Seq = NestedSequence<T>;
    ScopedNestedSequence(Seq& seq, SeqType type):
        seq_{ seq }
    { seq_.emplace_back(BeginSeqMark{type}); }

    ~ScopedNestedSequence()
    { seq_.emplace_back(EndSeqMark{}); }

private:
    Seq& seq_;
};

} // namespace gc::detail
