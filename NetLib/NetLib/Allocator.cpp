#include "pch.h"
#include "Allocator.h"

MemoryManager gMemoryManager;

void* PoolAllocator::Allocate(unsigned int allocSize)
{
	if (allocSize > 2048)
	{
		MemoryHeader* ptr = static_cast<MemoryHeader*>(malloc(allocSize + sizeof(MemoryHeader)));
		void* result = MemoryHeader::AttachHeader(ptr, allocSize);
		return result;
	}
	MemoryHeader* ptr = gMemoryManager.Alloc(allocSize);
	void* result = MemoryHeader::AttachHeader(ptr, allocSize);
	return result;
}

void PoolAllocator::Release(void* ptr)
{
	MemoryHeader* release = MemoryHeader::DetachHeader(ptr);
	if (release->allocSize > 2048)
	{
		free(release);
		return;
	}
	gMemoryManager.Release(release);
}
