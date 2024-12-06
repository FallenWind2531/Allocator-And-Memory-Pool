#include "memory_pool.h"
#include <iostream>

Block* MemoryPool::freeList[LISTNUM] = {nullptr};
PoolHead* MemoryPool::currentPool = nullptr;
char* MemoryPool::startAddress = nullptr;
char* MemoryPool::endAddress = nullptr;

void *MemoryPool::allocate(size_t size) noexcept {
    Block *tmp;

    if (size > MAXBLOCKSIZE) {
        tmp = (Block *)malloc(size * sizeof(char));
        if (tmp == nullptr) {
            std::cerr << "Out of memory" << std::endl;
            return nullptr;
        }
        return tmp;
    }

    int index = findIndex(size);
    if (freeList[index] == nullptr) {
        refill(index);
    }
    tmp = freeList[index];
    freeList[index] = freeList[index]->next;
    return tmp;
}

void MemoryPool::deallocate(void* ptr, size_t size) noexcept {
    if (size > MAXBLOCKSIZE) {
        free((char *)ptr);
        return;
    }

    int index = findIndex(size);
    Block *tmp = (Block *)ptr;
    tmp->next = freeList[index];
    freeList[index] = tmp;
}

void MemoryPool::clear() {
    PoolHead *tmp, *cur;
    cur = currentPool;
    while (cur != nullptr) {
        tmp = cur;
        cur = cur->next;
        free(tmp);
    }
}

void MemoryPool::addPool() {
    if (startAddress != nullptr) {
        size_t leftBytes = endAddress - startAddress;
        while (leftBytes > 0) {
            size_t index = findIndex(leftBytes) - 1; //?
            if (index < 0) {
                break;
            }
            Block *tmp = (Block *)startAddress;
            tmp->next = freeList[index];
            freeList[index] = tmp;
            startAddress += blockSize[index];
            leftBytes -= blockSize[index];
        }
    }

    char* newPool = (char *)malloc(sizeof(PoolHead) + POOLSIZE * sizeof(char));
    if (newPool == nullptr) {
        std::cerr << "Out of memory" << std::endl;
        return;
    }
    ((PoolHead *)newPool)->next = nullptr;
    if (currentPool == nullptr) {
        currentPool = (PoolHead *)newPool;
    } else {
        currentPool->next = (PoolHead *)newPool;
        currentPool = currentPool->next;
    }
    startAddress = newPool + sizeof(PoolHead);
    endAddress = startAddress + POOLSIZE;
}

size_t MemoryPool::findIndex(size_t size) {
    return (size + ALIGN - 1) / ALIGN - 1;
}

void MemoryPool::refill(size_t index) {
    size_t num = 16, leftBytes = endAddress - startAddress, size = blockSize[index];
    if (leftBytes < size) {
        addPool();
    } else if (leftBytes < num * size) {
        num = leftBytes / size;
    }
    for (int i = 0; i < num; i++) {
        Block *tmp = (Block *)startAddress;
        tmp->next = freeList[index];
        freeList[index] = tmp;
        startAddress += size;
    }
}