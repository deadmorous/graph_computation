/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "plot_visual/live_time_series.hpp"

#include "common/ring_buffer.hpp"
#include "common/type.hpp"

#include <algorithm>
#include <limits>
#include <numeric>
#include <vector>


namespace plot {

namespace {

template <typename T>
struct MinMax final
{
    T min{std::numeric_limits<T>::max()};
    T max{std::numeric_limits<T>::min()};
};

template <typename T>
auto combine(const MinMax<T>& a, const MinMax<T>& b) -> MinMax<T>
{
    return {
        .min = std::min(a.min, b.min),
        .max = std::max(a.max, b.max)
    };
}

template <typename T>
constexpr auto accum(const MinMax<T>& min_max,
                     std::type_identity_t<T> value) noexcept
    -> MinMax<T>
{
    return {
        std::max(min_max.max, value),
        std::max(min_max.min, value)
    };
}

template <typename T>
constexpr auto init_min_max(
    std::type_identity_t<T> value, common::Type_Tag<T> = {}) noexcept
    -> MinMax<T>
{
    return {
        .min = value,
        .max = value
    };
}

template <typename T>
constexpr auto is_valid(const MinMax<T>& min_max) noexcept -> bool
{
    return min_max.min <= min_max.max;
}

template <typename T>
constexpr auto compute_min_max(std::span<const T> values) noexcept
    -> MinMax<T>
{
    return std::accumulate(
        values.begin(),
        values.end(),
        MinMax<T>{},
        [](const MinMax<T>& acc, const T& value)
            { return accum(acc, value); });
}

std::vector<double> values;

} // anonymous namespace

class LiveTimeSeries::Impl final
{
public:
    auto frame_capacity() const noexcept -> size_t
    {
        return frames_.capacity();
    }

    auto set_frame_capacity(size_t capacity) -> void
    {
        frames_ = common::RingBuffer<Frame>{capacity};
    }

    auto add(std::span<const double> values) -> void
    {
        values = store(values); // NOTE: Result depends on next_ordinal_

        frames_.push_back({
            .user_frame {
                .ordinal = next_ordinal_++,
                .values = values },
            .min_max = compute_min_max(values)
        });

        min_max = std::accumulate(
            frames_.begin(),
            frames_.end(),
            MinMax<double>{},
            [](const MinMax<double>& acc, const Frame& frame)
            { return combine(acc, frame.min_max); });
    }

    auto clear() -> void
    {
        frames_.clear();
        value_buffer_.clear();
        values_per_frame_ = 0;
        next_ordinal_ = 0;
    }

    struct Frame final
    {
        LiveTimeSeries::Frame user_frame;
        MinMax<double> min_max;
    };
    using FrameBuffer = common::RingBuffer<Frame>;

    auto frames() noexcept -> const FrameBuffer&
    { return frames_; }

private:
    auto store(std::span<const double> values) -> std::span<const double>
    {
        if (frames_.capacity() == 0)
            throw std::runtime_error(
                "Trying to add a frame to time series object of zero capacity");

        if (values.size() != values_per_frame_)
        {
            values_per_frame_ = values.size();
            value_buffer_.resize(values_per_frame_ * frames_.capacity());
        }

        auto index = (next_ordinal_ % frames_.capacity()) * values_per_frame_;
        auto result =
            std::span<double>{value_buffer_.data() + index, values_per_frame_};
        std::ranges::copy(values, result.begin());
        return result;
    }

    std::vector<double> value_buffer_;
    size_t values_per_frame_{};
    FrameBuffer frames_{500};
    MinMax<double> min_max;
    size_t next_ordinal_{};
};


class LiveTimeSeries::Frames::iterator::Impl final
{
public:
    Impl() = default;

    Impl(LiveTimeSeries::Impl::FrameBuffer::const_iterator it) :
        it_{it}
    {}

    auto frame() const -> const LiveTimeSeries::Frame&
    { return it_->user_frame; }

    auto increment() -> void
    { ++it_; }

    auto operator==(const Impl& that) const noexcept -> bool
    { return it_ == that.it_; }

private:
    LiveTimeSeries::Impl::FrameBuffer::const_iterator it_;
};

LiveTimeSeries::Frames::iterator::iterator() = default;

LiveTimeSeries::Frames::iterator::~iterator() = default;

auto LiveTimeSeries::Frames::iterator::operator*() const -> reference
{
    return impl_->frame();
}

auto LiveTimeSeries::Frames::iterator::operator->() -> pointer
{
    return &impl_->frame();
}

auto LiveTimeSeries::Frames::iterator::operator++() -> iterator&
{
    impl_->increment();
    return *this;
}

auto LiveTimeSeries::Frames::iterator::operator++(int) -> iterator {
    auto impl_copy = *impl_;
    impl_->increment();
    return iterator{impl_copy};
}

bool operator==(const LiveTimeSeries::Frames::iterator& a,
                const LiveTimeSeries::Frames::iterator& b)
{ return *a.impl_ == *b.impl_; }

bool operator!=(const LiveTimeSeries::Frames::iterator& a,
                const LiveTimeSeries::Frames::iterator& b)
{ return !(*a.impl_ == *b.impl_); }

LiveTimeSeries::Frames::iterator::iterator(Impl impl)
{ impl_.emplace(impl); }



class LiveTimeSeries::Frames::Impl final
{
public:
    Impl(const LiveTimeSeries::Impl::FrameBuffer& frames) :
        frames_{frames}
    {}

    auto frames() const noexcept -> const LiveTimeSeries::Impl::FrameBuffer&
    { return frames_; }

private:
    const LiveTimeSeries::Impl::FrameBuffer& frames_;
};


LiveTimeSeries::Frames::~Frames() = default;

auto LiveTimeSeries::Frames::front() const -> const Frame&
{ return impl_->frames().front().user_frame; }

auto LiveTimeSeries::Frames::back() const -> const Frame&
{ return impl_->frames().back().user_frame; }

auto LiveTimeSeries::Frames::begin() const -> const_iterator
{ return const_iterator{ iterator::Impl{ impl_->frames().begin() } }; }

auto LiveTimeSeries::Frames::end() const -> const_iterator
{ return const_iterator{ iterator::Impl{ impl_->frames().end() } }; }

auto LiveTimeSeries::Frames::size() const -> size_t
{ return impl_->frames().size(); }

auto LiveTimeSeries::Frames::empty() const noexcept -> bool
{ return impl_->frames().empty(); }

LiveTimeSeries::Frames::Frames(Impl impl)
{ impl_.emplace(impl); }


LiveTimeSeries::LiveTimeSeries() :
    impl_{std::make_unique<Impl>()}
{
}

LiveTimeSeries::~LiveTimeSeries() = default;

auto LiveTimeSeries::frame_capacity() const noexcept -> size_t
{ return impl_->frame_capacity(); }

auto LiveTimeSeries::set_frame_capacity(size_t capacity) -> void
{ impl_->set_frame_capacity(capacity); }

auto LiveTimeSeries::add(std::span<const double> values) -> void
{ impl_->add(values); }

auto LiveTimeSeries::clear() -> void
{ impl_->clear(); }

auto LiveTimeSeries::frames() const -> Frames
{ return Frames{ Frames::Impl{ impl_->frames() } }; }

} // namespace plot
