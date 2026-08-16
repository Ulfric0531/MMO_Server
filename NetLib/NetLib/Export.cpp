#include "pch.h"
#include "MemoryManager.h"

#ifndef DLL_PROC
#define DLL_PROC extern "C" __declspec(dllexport)
#endif

DLL_PROC void* Allocate(unsigned int allocSize)
{
	return PoolAllocator::Allocate(allocSize);
}

DLL_PROC void Release(void* ptr)
{
	PoolAllocator::Release(ptr);
}
