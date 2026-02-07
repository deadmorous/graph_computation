/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#pragma once

#include "plot_visual/coordinate_range.hpp"

#include "common/checkpoint.hpp"
#include "common/fast_pimpl.hpp"

#include <memory>
#include <optional>
#include <span>


namespace plot {

class LiveTimeSeries final
{
public:
    // Describes changes since last checkpoint.
    // Knowing it in addition to the current state is sufficient
    // to update any state dependent on the state of this object.
    struct UpdateHistory final
    {
        // If a value is set, it is how many frames, up to frame capacity,
        // were added since last checkpoint (which is when history was consumed
        // last time). If not set, indicates that this object changed
        // completely and dependent state needs to be reloaded / recalculated.
        std::optional<size_t> frames_added;
        auto clear() -> void { frames_added = 0; }
        auto reset() -> void { frames_added.reset(); }
    };

    using Checkpoint = common::Checkpoint<UpdateHistory>;

    struct Frame final
    {
        size_t ordinal{};
        std::span<const double> values;
        CoordinateRange<double> value_range;
    };

    class Frames final
    {
    public:
        class iterator final {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = const Frame;
            using difference_type = std::ptrdiff_t;
            using index_type = size_t;
            using pointer = const Frame*;
            using reference = const Frame&;

            iterator();
            ~iterator();

            reference operator*() const;
            pointer operator->();

            iterator& operator++();
            iterator& operator--();

            iterator operator++(int);
            iterator operator--(int);

            iterator& operator+=(difference_type d) noexcept;
            iterator& operator-=(difference_type d) noexcept;

            friend iterator operator+(iterator it, difference_type d) noexcept;
            friend iterator operator-(iterator it, difference_type d) noexcept;


            friend difference_type operator-(iterator lhs,
                                             iterator rhs) noexcept;

            reference operator[](difference_type d);

            friend auto operator<=>(
                const iterator& a, const iterator& b) noexcept
                -> std::strong_ordering;

            friend bool operator==(const iterator& a, const iterator& b);

        private:
            friend class Frames;
            class Impl;

            explicit iterator(Impl);

            common::FastPimpl<Impl, 32> impl_;
        };

        using const_iterator = iterator;

        Frames();
        ~Frames();

        auto front() const -> const Frame&;
        auto back() const -> const Frame&;
        auto operator[](size_t index) const -> const Frame&;

        auto begin() const -> const_iterator;
        auto end() const -> const_iterator;
        auto size() const -> size_t;
        auto empty() const noexcept -> bool;

    private:
        friend class LiveTimeSeries;
        class Impl;

        explicit Frames(Impl);

        common::FastPimpl<Impl, 8> impl_;
    };


    LiveTimeSeries();
    ~LiveTimeSeries();

    auto frame_capacity() const noexcept -> size_t;
    auto set_frame_capacity(size_t capacity) -> void;

    auto add(std::span<const double> values) -> void;

    auto clear() -> void;

    auto frames() const -> Frames;

    auto value_range() const noexcept -> CoordinateRange<double>;

    auto register_checkpoint(Checkpoint&) -> void;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace plot
