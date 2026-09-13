#pragma once
#include "MemoryManager.h"

class PoolAllocator
{
public:
	static void*	Allocate(unsigned int allocSize);

	static void		Release(void* ptr);
};

