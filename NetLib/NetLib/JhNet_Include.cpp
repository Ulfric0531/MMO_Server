#include "pch.h"
#define DLL_EXPORT
#include "JhNet_Include.h"

DLL_PROC void* jhnet_alloc(unsigned int allocSize)
{
	return PoolAllocator::Allocate(allocSize);
}

DLL_PROC void jhnet_release(void* ptr)
{
	PoolAllocator::Release(ptr);
}