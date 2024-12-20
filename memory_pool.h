#pragma once
#include <cstddef>  // std::size_t
#include <iostream>

#define POOLSIZE 65536  // 内存池大小
#define LISTNUM 16      // 内存块链表(freeList)个数
#define ALIGN 16        // 内存对齐
#define MAXBLOCKSIZE (LISTNUM * ALIGN)  // 最大内存块大小

const size_t blockSize[LISTNUM] = {
    16, 32, 48, 64, 80, 96, 112, 128,
    144, 160, 176, 192, 208, 224, 240, 256  // 16种内存块大小
};

union Block {       // 内存块定义
    union Block *next;  //有对象时储存对象的值，被释放后内存块作为指针加入freeList指向下一个内存块
    char value[1];
};

typedef struct poolhead {   // 内存池头部
    struct poolhead *next;
} PoolHead;

class MemoryPool {
    public:
        static void *allocate(size_t size) noexcept;    // 内存分配
        static void deallocate(void* ptr, size_t size) noexcept;  // 内存释放
        static void clear();    // 清空内存池

    private:
        MemoryPool() {} // 禁止实例化
        static void addPool();  // 添加内存池
        static size_t findIndex(size_t size);   // 查找内存块大小对应的链表索引
        static void refill(size_t index);   // 重新填充内存块链表(freeList)

        static Block *freeList[LISTNUM];    // 内存块链表
        static PoolHead *currentPool;       // 当前内存池
        static char *startAddress;          // 内存池起始地址
        static char *endAddress;            // 内存池结束地址
};