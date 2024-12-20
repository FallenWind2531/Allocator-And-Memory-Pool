#include "memory_pool.h"
#include <iostream>

// 初始化静态成员变量
Block* MemoryPool::freeList[LISTNUM] = {nullptr};
PoolHead* MemoryPool::currentPool = nullptr;
char* MemoryPool::startAddress = nullptr;
char* MemoryPool::endAddress = nullptr;

void *MemoryPool::allocate(size_t size) noexcept {  // 内存分配
    Block *tmp;     

    if (size > MAXBLOCKSIZE) {  // 需分配内存大于最大的内存块大小
        tmp = (Block *)malloc(size * sizeof(char)); // 直接调用malloc分配内存
        if (tmp == nullptr) {   // 分配失败
            std::cerr << "Out of memory" << std::endl;
            return nullptr;
        }
        return tmp;
    }

    int index = findIndex(size);    // 查找内存块大小对应的链表索引
    if (freeList[index] == nullptr) {   // 内存块链表为空
        refill(index);  // 重新填充内存块链表
    }
    tmp = freeList[index];  // 从内存块链表中取出一个内存块
    freeList[index] = freeList[index]->next;    // 更新内存块链表
    return tmp;
}

void MemoryPool::deallocate(void* ptr, size_t size) noexcept {
    if (size > MAXBLOCKSIZE) {  // 需释放内存大于最大的内存块大小
        free((char *)ptr);  // 直接调用free释放内存
        return;
    }

    int index = findIndex(size);    // 查找内存块大小对应的链表索引
    Block *tmp = (Block *)ptr;  
    tmp->next = freeList[index];    
    freeList[index] = tmp;  // 将释放出的内存块加入对应大小内存块链表
}

void MemoryPool::clear() {  // 清空内存池
    PoolHead *tmp, *cur;
    cur = currentPool;
    while (cur != nullptr) {    // 释放所有内存池
        tmp = cur;
        cur = cur->next;
        free(tmp);
    }
}

void MemoryPool::addPool() {    // 添加内存池
    if (startAddress != nullptr) {  // 内存池不为空
        size_t leftBytes = endAddress - startAddress;   // 计算剩余内存
        while (leftBytes > 0) {
            size_t index = findIndex(leftBytes) - 1;    // 查找剩余内存对应的内存块大小
            if (index < 0) {
                break;
            }
            Block *tmp = (Block *)startAddress;
            tmp->next = freeList[index];    
            freeList[index] = tmp;  // 将剩余内存加入对应大小内存块链表
            startAddress += blockSize[index];   // 更新内存池起始地址
            leftBytes -= blockSize[index];  // 更新剩余内存
        }
    }

    char* newPool = (char *)malloc(sizeof(PoolHead) + POOLSIZE * sizeof(char));   // 申请新的内存池
    if (newPool == nullptr) {   // 申请失败
        std::cerr << "Out of memory" << std::endl;
        return;
    }
    ((PoolHead *)newPool)->next = nullptr;
    if (currentPool == nullptr) {   
        currentPool = (PoolHead *)newPool;
    } else {    // 将新的内存池加入内存池链表
        ((PoolHead *)newPool)->next = currentPool;
        currentPool = (PoolHead *)newPool;
    }
    startAddress = newPool + sizeof(PoolHead);  // 更新内存池起始地址
    endAddress = startAddress + POOLSIZE;   // 更新内存池结束地址
}

size_t MemoryPool::findIndex(size_t size) {   // 查找内存块大小对应的链表索引
    return (size + ALIGN - 1) / ALIGN - 1;
}

void MemoryPool::refill(size_t index) {  // 重新填充内存块链表(freeList)
    size_t num = 16, leftBytes = endAddress - startAddress, size = blockSize[index];    // 每次填充16个内存块
    if (leftBytes < size) { // 剩余内存不足一个内存块大小
        addPool();  // 添加新的内存池
    } else if (leftBytes < num * size) {    // 剩余内存不足16个内存块大小
        num = leftBytes / size; // 计算剩余内存能填充的内存块个数
    }
    for (int i = 0; i < num; i++) { 
        Block *tmp = (Block *)startAddress;
        tmp->next = freeList[index];
        freeList[index] = tmp;  // 将新的内存块加入对应大小内存块链表
        startAddress += size;   // 更新内存池起始地址
    }
}