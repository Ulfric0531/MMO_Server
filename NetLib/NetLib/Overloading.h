#pragma once
struct NewDelete
{
public:
	NewDelete() = default;
	static void* operator new(size_t size)
	{
		PoolAllocator::Allocate(size);
	}

	static void operator delete(void* ptr)
	{
		PoolAllocator::Release(ptr);
	}
};