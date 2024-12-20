#pragma once
#include "memory_pool.h"
#include <cstddef>  // std::size_t

namespace mem {
    template <class T>
    class allocator {
        public:
            // 以下是标准库allocator的定义
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
                return reinterpret_cast<pointer>(MemoryPool::allocate(n * sizeof(value_type))); // 调用内存池分配内存
            }

            void deallocate(pointer p, size_type n) {
                MemoryPool::deallocate((void *)p, n * sizeof(value_type));  // 调用内存池释放内存
            }

            size_type max_size() const noexcept {
                size_type maxSize = 2ULL * 1024ULL * 1024ULL * 1024ULL; // 假设最大内存容量为2GB
                return (maxSize / sizeof(value_type));  // 返回最大可分配的元素个数
            }

            pointer address(reference x) const {
                return &x;  
            }

            const_pointer address(const_reference x) const {
                return &x;  
            }

            template <class U, class... Args>
            void construct(U* p, Args&&... args) {  // 在p指向的内存上构造对象
                ::new ((void *)p) U(std::forward<Args>(args)...);
            }

            template <class U>
            void destroy(U* p) {    // 析构p指向的对象
                p->~U();
            }
    };
} // namespace mem