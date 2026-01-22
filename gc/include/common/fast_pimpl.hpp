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


#include <cstddef>
#include <new>
#include <utility>
#include <cassert>

/* NOTE: The implementation is vibe-coded using Google Gemini Pro, with minor
 * manual editing afterwards.
 *
 * Here is the prompt:
 *
 * I bet you've heard about the PIMPL pattern and how it's traditionally
 * implemented in C++ - using a unique_ptr. I want a similar thing, but
 * allocation-free. Can you generate something suitable, please? We need:
 * Compile-time-sized opaque container, perhaps parameterized by
 * forward-declared implementation type; storage size is to be guessed by the
 * user, since it's declared as a field of a class, but with a compile-time
 * check that the type fits into the storage. Then we need methods to
 * initialize, access, and destroy the value stored These methods are available
 * in the translation unit, where actual type is known.
 *
 * ...
 *
 * So nice, thanks! May we have a unit test for this, please? Assume we use
 * googletest for unit testing.
 *
 * ... - for the tests, see test_fast_pimpl.cpp ...
 */

namespace common {

/**
 * FastPimpl
 * * An allocation-free, cache-friendly Pointer-to-Implementation wrapper.
 * * @tparam T The implementation type (can be incomplete in the header).
 * @tparam Size The size of the internal storage in bytes (must be >= sizeof(T)).
 * @tparam Alignment The alignment of the storage (must be >= alignof(T)).
 */
template <typename T, std::size_t Size, std::size_t Alignment = alignof(void*)>
class FastPimpl {
public:
    // Default constructor: creates the shell, but does NOT construct T.
    // T is usually not known here. construction must be done via emplace().
    FastPimpl() noexcept : initialized_(false) {}

    // Destructor: Automatically destroys T if it was constructed.
    // Note: This requires T to be a complete type at the point where
    // the destructor is instantiated (typically in the .cpp file).
    ~FastPimpl() {
        destroy();
    }

    // Prevent copying to avoid slicing or complex logic without knowing T
    FastPimpl(const FastPimpl&) = delete;
    FastPimpl& operator=(const FastPimpl&) = delete;

    // Move could be implemented, but requires T to be complete to call T(T&&).
    // For strict PIMPL safety, we delete it here, but it can be enabled
    // if the user defines the move ctor in the implementation file.
    FastPimpl(FastPimpl&&) = delete;
    FastPimpl& operator=(FastPimpl&&) = delete;

    /**
     * Constructs the implementation object in the internal storage.
     * This method must be called in the .cpp file where T is defined.
     * * Triggers compile-time checks for Size and Alignment.
     */
    template <typename... Args>
    void emplace(Args&&... args) {
        // COMPILE-TIME CHECKS
        // These assertions fire only when emplace is instantiated (in the .cpp)
        static_assert(sizeof(T) <= Size,
                      "FastPimpl Error: Storage Size is too small for the Implementation type.");
        static_assert(alignof(T) <= Alignment,
                      "FastPimpl Error: Alignment requirement of T is stricter than storage.");

        assert(!initialized_ && "FastPimpl already initialized!");

        new (&storage_) T(std::forward<Args>(args)...);
        initialized_ = true;
    }

    /**
     * Manually destroys the object.
     * Usually not needed as the destructor handles this, but provided for flexibility.
     */
    void destroy() {
        if (initialized_) {
            // We interpret the storage as T* to call the destructor
            // strict aliasing is satisfied because we placement-new'd here.
            ptr()->~T();
            initialized_ = false;
        }
    }

    // Accessors
    T* operator->() { return ptr(); }
    const T* operator->() const { return ptr(); }

    T& operator*() { return *ptr(); }
    const T& operator*() const { return *ptr(); }

    // Check if the PIMPL is currently holding a constructed object
    bool has_value() const { return initialized_; }

private:
    // Helper to get typed pointer.
    // std::launder is technically required in C++17 for correct aliasing analysis
    // after placement new, though most compilers work without it for PODs.
    T* ptr() {
        return std::launder(reinterpret_cast<T*>(&storage_));
    }

    const T* ptr() const {
        return std::launder(reinterpret_cast<const T*>(&storage_));
    }

    // The opaque storage
    alignas(Alignment) std::byte storage_[Size];
    bool initialized_;
};

} // namespace common
