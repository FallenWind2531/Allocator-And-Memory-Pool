#pragma once
#include "memory_pool.h"
#include <cstddef>

namespace mem {
    template <class T, size_t BlockSize = 4096>
    class allocator {
        public:
            typedef T value_type;
            typedef value_type* pointer;
            typedef const value_type* const_pointer;
            typedef value_type& reference;
            typedef const value_type& const_reference;
            typedef std::size_t size_type;
            typedef std::ptrdiff_t difference_type;

            template <class U>
            struct rebind {
                typedef allocator<U> other;
            };

            allocator() noexcept {}
            allocator(const allocator& other) noexcept {}
            template <class U>
            allocator(const allocator<U>& other) noexcept {}

            ~allocator() {}

            pointer allocate(size_type n, const void* hint = 0) {
                return reinterpret_cast<pointer>(MemoryPool::allocate(n * sizeof(value_type)));
            }

            void deallocate(pointer p, size_type n) {
                MemoryPool::deallocate((void *)p, n * sizeof(value_type));
            }

            size_type max_size() const noexcept {
                size_type maxSize = 2ULL * 1024ULL * 1024ULL * 1024ULL;
                return (maxSize / sizeof(value_type));
            }

            pointer address(reference x) const {
                return &x;
            }

            const_pointer address(const_reference x) const {
                return &x;
            }

            template <class U, class... Args>
            void construct(U* p, Args&&... args) {
                ::new ((void *)p) U(std::forward<Args>(args)...);
            }

            template <class U>
            void destroy(U* p) {
                p->~U();
            }
    };
} // namespace mem