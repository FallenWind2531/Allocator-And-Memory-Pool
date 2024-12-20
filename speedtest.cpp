#include <cassert>	
#include <ctime>	//记录运行时间
#include <iostream> 
#include <vector>	
#include <list>

#include "allocator.h"
#include "memory_pool.h" 


#define ELEMS 10000     // 插入元素个数
#define TIMES 1000       // 重复次数

int main()
{
	clock_t start, end;

	// STL 默认allocator
	std::list<int, std::allocator<int> > stackDefault;
	start = clock();
	for (int j = 0; j < TIMES; j++)
	{
		assert(stackDefault.empty());   // 置空
		for (int i = 0; i < ELEMS; i++)
		{
			stackDefault.push_back(i);
		}
		
		for (int i = 0; i < ELEMS; i++)
		{
			stackDefault.pop_back();
		}
	}
	std::cout << "Default Allocator Time: ";
	std::cout << (((double)clock() - start) / CLOCKS_PER_SEC) << "\n\n";

	// memory_pool allocator
	std::list<int, mem::allocator<int> > stackPool;
	start = clock();
	for (int j = 0; j < TIMES; j++)
	{
		assert(stackPool.empty());  // 置空
		for (int i = 0; i < ELEMS; i++)
		{
			stackPool.push_back(i);
		}
		
		for (int i = 0; i < ELEMS; i++)
		{
			stackPool.pop_back();
		}
	}
	std::cout << "MemoryPool Allocator Time: ";
	std::cout << (((double)clock() - start) / CLOCKS_PER_SEC) << "\n\n";

	MemoryPool::clear();    // 清空内存池

	return 0;
}