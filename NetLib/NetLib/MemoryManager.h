#pragma once
#include "MemoryPool.h"

#define MAX_BUCKET_SIZE (4096)

class MemoryManager
{
public:
	MemoryManager();
	~MemoryManager();

	MemoryHeader*	Alloc(unsigned int bucketSize);
	void			Release(MemoryHeader* ptr);
private:
	MemoryPool*		_memoryPools[MAX_BUCKET_SIZE + 1] = { nullptr, };
};
