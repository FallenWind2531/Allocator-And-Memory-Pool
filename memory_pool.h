#pragma once
#include <cstddef>
#include <iostream>

#define POOLSIZE 65536
#define LISTNUM 16
#define ALIGN 16
#define MAXBLOCKSIZE (LISTNUM * ALIGN)

const size_t blockSize[LISTNUM] = {
    16, 32, 48, 64, 80, 96, 112, 128,
    144, 160, 176, 192, 208, 224, 240, 256
};

union Block {
    union Block *next;
    char value[1];
};

typedef struct poolhead {
    struct poolhead *next;
} PoolHead;

class MemoryPool {
    public:
        static void *allocate(size_t size) noexcept;
        static void deallocate(void* ptr, size_t size) noexcept;
        static void clear();

    private:
        MemoryPool() {}
        static void addPool();
        static size_t findIndex(size_t size);
        static void refill(size_t index);

        static Block *freeList[LISTNUM];
        static PoolHead *currentPool;
        static char *startAddress;
        static char *endAddress;
};