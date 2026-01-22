/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once


#include <algorithm>
#include <cassert>
#include <iterator>
#include <memory>
#include <span>
#include <stdexcept>


/* NOTE: The implementation is vibe-coded using Google Gemini, with minor
 * manual editing afterwards.
 *
 * Here is the prompt:
 *
 * Hi, would you mind generating a ring buffer data structure in C++?
 * We name it RingBuffer. It will be a template class parameterized on element
 * type T. Buffer capacity should be specified in the constructor (it's not a
 * compile-time constant). I need methods to access range of elements as
 * std::span. We won't do any trick with memory mapping for now and hence will
 * have one method to extract elements from the logical beginning of the
 * sequence to buffer upper address boundary, and from the lower buffer boundary
 * to the logical end of buffer (either sequence can be empty).
 * We will add elements by one, always at the end. We also need the clear
 * method.
 *
 * ...
 *
 * Thanks, that's very helpful. And of course you can generate a unit test for
 * it, right? Please assume we are using googletest.
 *
 * ... - for the tests, see test_ring_buffer.cpp ...
 *
 * Well yes, an overload of push_back taking an rvalue reference would be good
 * to have. Also I absolutely need empty(), front() and back() methods, and at
 * least forward iterators (both const and non-const). Also, let's rename
 * get_first_segment to first_segment and get_second_segment to second_segment.
 * Can you please update RingBuffer and unit test and give me full updated code?
 *
 * ...
 *
 * Oh, yes, forgot about the pop_front. I do need it!
 */

namespace common {

template <typename T>
class RingBuffer {
public:
    // --- Iterator Implementation ---
    template <typename ValueType>
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = ValueType;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        Iterator(ValueType* data, size_t capacity, size_t index, size_t count)
            : m_data(data),
              m_capacity(capacity),
              m_index(index),
              m_count(count) {}

        reference operator*() const { return m_data[m_index % m_capacity]; }
        pointer operator->() { return &m_data[m_index % m_capacity]; }

        Iterator& operator++() {
            m_index++;
            m_count--;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const Iterator& a, const Iterator& b) {
            return a.m_count == b.m_count;
        }
        friend bool operator!=(const Iterator& a, const Iterator& b) {
            return !(a == b);
        }

    private:
        ValueType* m_data;
        size_t m_capacity;
        size_t m_index;
        size_t m_count;
    };

    using iterator = Iterator<T>;
    using const_iterator = Iterator<const T>;

    // --- Constructor ---
    explicit RingBuffer(size_t capacity)
        : m_capacity(capacity),
        m_data(std::make_unique<T[]>(capacity)),
        m_head(0),
        m_size(0) {
        if (capacity == 0) throw std::invalid_argument("Capacity must be > 0");
    }

    // --- Accessors ---
    T& front() { assert(m_size > 0); return m_data[m_head]; }
    const T& front() const { assert(m_size > 0); return m_data[m_head]; }

    T& back() {
        assert(m_size > 0); return m_data[(m_head + m_size - 1) % m_capacity]; }

    const T& back() const {
        assert(m_size > 0); return m_data[(m_head + m_size - 1) % m_capacity]; }

    // --- Modifiers ---
    void push_back(const T& value) { push_impl(value); }
    void push_back(T&& value) { push_impl(std::move(value)); }

    void pop_front() {
        if (m_size > 0) {
            // Explicitly overwrite to release resources if T is a complex type
            m_data[m_head] = T{};
            m_head = (m_head + 1) % m_capacity;
            m_size--;
        }
    }

    void clear() {
        // Overwrite elements to ensure destructors/resource release
        while (!empty()) {
            pop_front();
        }
        m_head = 0;
        m_size = 0;
    }

    // --- Segment Views ---
    std::span<T> first_segment() {
        if (m_size == 0) return {};
        size_t count = std::min(m_size, m_capacity - m_head);
        return {&m_data[m_head], count};
    }

    std::span<const T> first_segment() const {
        if (m_size == 0) return {};
        size_t count = std::min(m_size, m_capacity - m_head);
        return {&m_data[m_head], count};
    }

    std::span<T> second_segment() {
        if (m_size == 0) return {};
        size_t first_count = std::min(m_size, m_capacity - m_head);
        size_t second_count = m_size - first_count;
        return second_count == 0
                   ? std::span<T>{}
                   : std::span<T>{&m_data[0], second_count};
    }

    std::span<const T> second_segment() const {
        if (m_size == 0) return {};
        size_t first_count = std::min(m_size, m_capacity - m_head);
        size_t second_count = m_size - first_count;
        return second_count == 0
                   ? std::span<const T>{}
                   : std::span<const T>{&m_data[0], second_count};
    }

    // --- Iterators ---
    iterator begin() {
        return iterator(m_data.get(), m_capacity, m_head, m_size);
    }

    iterator end() {
        return iterator(m_data.get(), m_capacity, m_head + m_size, 0);
    }

    const_iterator begin() const {
        return const_iterator(m_data.get(), m_capacity, m_head, m_size);
    }

    const_iterator end() const {
        return const_iterator(m_data.get(), m_capacity, m_head + m_size, 0);
    }

    // --- Status ---
    size_t size() const { return m_size; }
    size_t capacity() const { return m_capacity; }
    bool empty() const { return m_size == 0; }

private:
    template <typename U>
    void push_impl(U&& value) {
        size_t index = (m_head + m_size) % m_capacity;
        m_data[index] = std::forward<U>(value);
        if (m_size < m_capacity) {
            m_size++;
        } else {
            m_head = (m_head + 1) % m_capacity;
        }
    }

    size_t m_capacity;
    std::unique_ptr<T[]> m_data;
    size_t m_head;
    size_t m_size;
};

} // namespace common
