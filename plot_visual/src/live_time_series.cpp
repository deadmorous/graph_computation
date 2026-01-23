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

#include <algorithm>
#include <numeric>
#include <vector>


namespace plot {

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
            .ordinal = next_ordinal_++,
            .values = values,
            .value_range = compute_coordinate_range(values)
        });

        value_range_ = std::accumulate(
            frames_.begin(),
            frames_.end(),
            CoordinateRange<double>{},
            [](const CoordinateRange<double>& acc, const Frame& frame)
            { return combine_coordinate_range(acc, frame.value_range); });
    }

    auto clear() -> void
    {
        frames_.clear();
        value_buffer_.clear();
        values_per_frame_ = 0;
        next_ordinal_ = 0;
    }

    using Frame = LiveTimeSeries::Frame;
    using FrameBuffer = common::RingBuffer<Frame>;

    auto frames() noexcept -> const FrameBuffer&
    { return frames_; }

    auto value_range() const noexcept -> CoordinateRange<double>
    { return value_range_; }

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
    CoordinateRange<double> value_range_;
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
    { return *it_; }

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
{ return impl_->frames().front(); }

auto LiveTimeSeries::Frames::back() const -> const Frame&
{ return impl_->frames().back(); }

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

auto LiveTimeSeries::value_range() const noexcept -> CoordinateRange<double>
{ return impl_->value_range(); }

} // namespace plot
