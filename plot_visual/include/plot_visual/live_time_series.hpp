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

#include "common/fast_pimpl.hpp"

#include <memory>
#include <span>


namespace plot {

class LiveTimeSeries final
{
public:
    struct Frame
    {
        size_t ordinal{};
        std::span<const double> values;
        CoordinateRange<double> value_range;
    };

    class Frames
    {
    public:
        class iterator final {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = const Frame;
            using difference_type = std::ptrdiff_t;
            using pointer = const Frame*;
            using reference = const Frame&;

            iterator();
            ~iterator();

            reference operator*() const;
            pointer operator->();

            iterator& operator++();

            iterator operator++(int);

            friend bool operator==(const iterator& a, const iterator& b);
            friend bool operator!=(const iterator& a, const iterator& b);

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

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace plot
