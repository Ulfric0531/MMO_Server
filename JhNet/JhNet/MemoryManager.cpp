#include "pch.h"
#include "MemoryManager.h"

extern MemoryManager gMemoryManager;

MemoryManager::MemoryManager()
{
	unsigned int bucketSize = 32;
	unsigned int sameBucket = 1;
	for (; bucketSize < 512; bucketSize += 32)
	{
		_memoryPools[bucketSize] = new MemoryPool(bucketSize);
		for (; sameBucket < bucketSize; ++sameBucket)
		{
			_memoryPools[sameBucket] = _memoryPools[bucketSize];
		}
	}

	++sameBucket;

	for (; bucketSize < 1024; bucketSize += 64)
	{
		_memoryPools[bucketSize] = new MemoryPool(bucketSize);
		for (; sameBucket < bucketSize; ++sameBucket)
		{
			_memoryPools[sameBucket] = _memoryPools[bucketSize];
		}
	}

	++sameBucket;

	for (; bucketSize <= 2048; bucketSize += 128)
	{
		_memoryPools[bucketSize] = new MemoryPool(bucketSize);
		for (; sameBucket < bucketSize; ++sameBucket)
		{
			_memoryPools[sameBucket] = _memoryPools[bucketSize];
		}
	}
}

MemoryManager::~MemoryManager()
{
}

MemoryHeader* MemoryManager::Alloc(unsigned int bucketSize)
{
	MemoryHeader* ptr = _memoryPools[bucketSize]->Pop();
	return ptr;
}

void MemoryManager::Release(MemoryHeader* ptr)
{
	_memoryPools[ptr->allocSize]->Push(ptr);
}
