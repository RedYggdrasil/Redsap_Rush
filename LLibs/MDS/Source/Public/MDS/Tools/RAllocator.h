#pragma once


#include <memory>
#include <xmmintrin.h>
#include <vector>

//Source : https://devblogs.microsoft.com/cppblog/the-mallocator/
//Also Microsoft Copilot for custom alignment
template <typename T, int Align>
class RAlignedAllocator {
public:
    // The following will be the same for virtually all allocators.
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    T* address(T& r) const {
        return &r;
    }

    const T* address(const T& s) const {
        return &s;
    }

    size_t max_size() const {
        // The following has been carefully written to be independent of
        // the definition of size_t and to avoid signed/unsigned warnings.
        return (static_cast<size_t>(0) - static_cast<size_t>(1)) / sizeof(T);
    }

    template<typename U>
    struct rebind {
        typedef RAlignedAllocator<U, Align> other;
    };

    void construct(T* const p, const T& t) const {
        void* const pv = static_cast<void*>(p);
        new (pv) T(t);
    }

    void destroy(T* const p) const; // Defined below.

    template <typename U>
    bool operator!=(const RAlignedAllocator<U, Align>& other) const {
        return !(*this == other);
    }

    // Returns true if and only if storage allocated from *this
    // can be deallocated from other, and vice versa.
    // Always returns true for stateless allocators.

    template <typename U>
    bool operator==(const RAlignedAllocator<U, Align>& other) const {
        return true;
    }

    // Default constructor, copy constructor, rebinding constructor, and destructor.
    // Empty for stateless allocators.
    RAlignedAllocator() noexcept {}

    template <typename U>
    RAlignedAllocator(const RAlignedAllocator<U, Align>&) noexcept {}

    ~RAlignedAllocator() { }

    T* allocate(std::size_t n) {
        if (auto p = static_cast<T*>(_mm_malloc(n * sizeof(T), Align))) return p;
        throw std::bad_alloc();
    }

    void deallocate(T* p, std::size_t) noexcept { _mm_free(p); }

    // The following will be the same for all allocators that ignore hints.
    template <typename U> T* allocate(const size_t n, const U* /* const hint */) const {
        return allocate(n);
    }
    // Allocators are not required to be assignable, so
    // all allocators should have a private unimplemented
    // assignment operator. Note that this will trigger the
    // off-by-default (enabled under /Wall) warning C4626
    // “assignment operator could not be generated because a
    // base class assignment operator is inaccessible” within
    // the STL headers, but that warning is useless.
private:
    template <typename U>
    RAlignedAllocator<U, Align>& operator=(const RAlignedAllocator<U, Align>&);
};

// A compiler bug causes it to believe that p->~T() doesn’t reference p.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#endif

// The definition of destroy() must be the same for all allocators.
template <typename T, int Align> void RAlignedAllocator<T, Align>::destroy(T* const p) const {
    p->~T();
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

template <typename T>
using RAligndAlloc16 = RAlignedAllocator<T, 16>;


