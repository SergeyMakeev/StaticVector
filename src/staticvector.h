#pragma once
#include <vector>
#include <assert.h>

#define _STATIC_VECTOR_UNUSED(x) (void)(x)

namespace stdext
{

    //
    // Statefull C++ STL allocator (C++11 only)
    //   has a embedded small buffer and uses it when possible
    //
    template <typename T, std::size_t N>
    class static_allocator
    {
        // Current allocator position
        T* cur = nullptr;

        // Embedded storage
        typename std::aligned_storage<sizeof(T), alignof(T)>::type storage[N];

    public:

        using value_type = T;
        using size_type = std::size_t;
        using propagate_on_container_move_assignment = std::true_type;

    public:
        template <class U> struct rebind { typedef static_allocator<U, N> other; };

        template <class U>
        static_allocator(const static_allocator<U, N>& alloc) noexcept
        {
            _STATIC_VECTOR_UNUSED(alloc);
        }

        static_allocator() noexcept
        {
        }

        static_allocator(const static_allocator&) noexcept
        {
        }

        static_allocator& operator=(const static_allocator&) = delete;

        // While allocator is not activated is just pass-trrough
        void activate()
        {
            cur = reinterpret_cast<T*>(&storage[0]);
        }

        T* allocate(std::size_t n)
        {
            T* end = reinterpret_cast<T*>(&storage[N]);
            if (cur != nullptr && cur + n <= end)
            {
                T* r = cur;
                cur += n;
                assert(r < end && "Sanity check failed!");
                return r;
            }
            return static_cast<T*>(::operator new(n * sizeof(T)));
        }

        void deallocate(T* ptr, std::size_t n)
        {
            T* begin = reinterpret_cast<T*>(&storage[0]);
            if (cur != nullptr && ptr >= begin && ptr <= cur)
            {
                // Deallocate only from the tail (otherwise ignore deallocation request)
                if (ptr + n == cur)
                {
                    cur = ptr;
                }
                return;
            }
            ::operator delete(ptr);
        }

        template <class T1, std::size_t N1, class U, std::size_t M>
        friend bool operator==(const static_allocator<T1, N1>& x, const static_allocator<U, M>& y);

        template <class U, std::size_t M> friend class static_allocator;
    };

    template <class T, std::size_t N, class U, std::size_t M>
    bool operator==(const static_allocator<T, N>& x, const static_allocator<U, M>& y)
    {
        return N == M && &x.storage == &y.storage;
    }

    template <class T, std::size_t N, class U, std::size_t M>
    bool operator!=(const static_allocator<T, N>& x, const static_allocator<U, M>& y)
    {
        return !(x == y);
    }

    //
    //   Vector with embedded small storage to minimize number of heap allocations
    //
    template<typename T, std::size_t N>
    class static_vector : public std::vector<T, static_allocator<T, N>>
    {
        typedef std::vector<T, static_allocator<T, N>> super;

        void initialize()
        {
            // Activate allocator
            this->_Getal().activate();

            // Reserve memory
            // Since we can't easily deallocate memory from built-in storage, we have to reserve all the memory in advance
            reserve(N);
        }

    public:

        static_vector()
        {
            initialize();
        }

        template<typename T0>
        explicit static_vector(T0&& p0) : super(p0)
        {
            initialize();
        }

        template<typename T0, typename T1>
        explicit static_vector(T0&& p0, T1&& p1) : super(p0, p1)
        {
            initialize();
        }

        template<typename T0, typename T1, typename T2>
        explicit static_vector(T0&& p0, T1&& p1, T2&& p2) : super(p0, p1, p2)
        {
            initialize();
        }
    };
}


#undef _STATIC_VECTOR_UNUSED
