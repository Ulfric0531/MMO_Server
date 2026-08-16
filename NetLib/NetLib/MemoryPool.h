#pragma once
#include "pch.h"
#include "stdlib.h"
#include "SpinLock.h"
#define CACHE_LINE (64)
#define MEM_PAGE_SIZE (4096)
#define EXPECTED_ARENA_SIZE (1024 * 1024)

struct MemoryHeader : public SLIST_ENTRY // temp
{
public:
	static void*			AttachHeader(void* ptr, unsigned int size);

	static MemoryHeader*	DetachHeader(void* ptr);

public:
	unsigned int allocSize;
	MemoryHeader* next;
};

struct PoolHead
{
	MemoryHeader* next;
};

struct Arena
{
	char* arena;
	Arena* next;
};

struct ArenaHead
{
	Arena* next;
};

class MemoryPool
{
public:
	MemoryPool(unsigned int bucketSize);
	MemoryPool(unsigned int bucketSize, unsigned int reserveCount);
	~MemoryPool();

	void				Push(MemoryHeader* ptr);
	MemoryHeader*		Pop();
	unsigned int		CalculateArenaSize();
	char*				CommitArena();

	static void* operator new(size_t size)
	{
		return _aligned_malloc(size, CACHE_LINE);
	}

	static void operator delete(void* ptr)
	{
		_aligned_free(ptr);
	}

private:
	alignas(CACHE_LINE) volatile long _poolLock;
	alignas(CACHE_LINE) volatile long _arenaLock;

	SLIST_HEADER	_header; // temp
	unsigned int	_freeCount;
	unsigned int	_allocCount;
	unsigned int	_bucketSize;
	unsigned int	_arenaPos;
	unsigned int	_arenaSize;
	unsigned int	_arenaCount;
	PoolHead		_poolHead;
	ArenaHead		_arenaHead;

};

