#include "pch.h"
#include "MemoryPool.h"

// --------------------
//     MemoryHeader
// --------------------
void* MemoryHeader::AttachHeader(void* ptr, unsigned int size)
{
	MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr);
	header->allocSize = size;

	return reinterpret_cast<void*>(++header);
}

MemoryHeader* MemoryHeader::DetachHeader(void* ptr)
{
	return reinterpret_cast<MemoryHeader*>(ptr) - 1;
}


// --------------------
//      MemoryPool
// --------------------

MemoryPool::MemoryPool(unsigned int bucketSize)
	: _freeCount(0)
	, _allocCount(0)
	, _bucketSize(bucketSize)
	, _poolHead({ 0, })
	, _arenaHead({0, })
	, _arenaPos(0)
	, _arenaCount(1)
	, _poolLock(0)
	, _arenaLock(0)
{
	_arenaSize = CalculateArenaSize();
	Arena* newArena = new Arena();
	newArena->next = nullptr;
	newArena->arena = CommitArena();
	_arenaHead.next = newArena;

	// InitializeSListHead(&_header); // temp
}

MemoryPool::MemoryPool(unsigned int bucketSize, unsigned int reserveCount)
	: _freeCount(0)
	, _allocCount(0)
	, _bucketSize(bucketSize)
	, _poolHead({ 0, })
	, _arenaHead({ 0, })
	, _arenaPos(0)
	, _arenaCount(1)
	, _poolLock(0)
	, _arenaLock(0)
{
	// TODO : 메모리풀 생성시 미리 노드를 reserveCount만큼 생성
	_arenaSize = CalculateArenaSize();
}

MemoryPool::~MemoryPool()
{
	// TODO
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	
	AcquireSpinLock(&_poolLock);
	ptr->next = _poolHead.next;
	_poolHead.next = ptr;
	++_freeCount;
	ReleaseSpinLock(&_poolLock);
	
	// InterlockedPushEntrySList(&_header, (SLIST_ENTRY*)(ptr));
}

MemoryHeader* MemoryPool::Pop()
{
	/*
	MemoryHeader* header = (MemoryHeader*)InterlockedPopEntrySList(&_header); // temp;
	if (header == nullptr)
	{
		AcquireSpinLock(&_arenaLock);
		void* ptr = _arenaHead.next->arena + _arenaPos;
		_arenaPos += _bucketSize + sizeof(MemoryHeader);
		if (_arenaSize - _arenaPos < (_bucketSize + sizeof(MemoryHeader)))
		{
			Arena* newArena = new Arena();
			newArena->arena = CommitArena();
			newArena->next = _arenaHead.next;
			_arenaHead.next = newArena;
			_arenaPos = 0;
		}
		ReleaseSpinLock(&_arenaLock);
		return reinterpret_cast<MemoryHeader*>(ptr);
	}
	*/
	
	AcquireSpinLock(&_poolLock);
	MemoryHeader* header = _poolHead.next;
	if (header == nullptr)
	{
		void* ptr = _arenaHead.next->arena + _arenaPos;
		_arenaPos += _bucketSize + sizeof(MemoryHeader);
		if (_arenaSize - _arenaPos < (_bucketSize + sizeof(MemoryHeader)))
		{
			Arena* newArena = new Arena();
			newArena->arena = CommitArena();
			newArena->next = _arenaHead.next;
			_arenaHead.next = newArena;
			_arenaPos = 0;
		}
		ReleaseSpinLock(&_poolLock);
		return reinterpret_cast<MemoryHeader*>(ptr);
	}
	_poolHead.next = header->next;
	--_freeCount;
	ReleaseSpinLock(&_poolLock);
	

	return header;
}

unsigned int MemoryPool::CalculateArenaSize()
{
	unsigned int a = _bucketSize + sizeof(MemoryHeader);
	unsigned int b = MEM_PAGE_SIZE;

	while (b != 0)
	{
		unsigned int temp = b;
		b = a % b;
		a = temp;
	}
	unsigned int minArenaSize = ((_bucketSize + sizeof(MemoryHeader)) * MEM_PAGE_SIZE) / a;
	unsigned int result = ((EXPECTED_ARENA_SIZE) / minArenaSize) * minArenaSize;
	return result;
}

char* MemoryPool::CommitArena()
{
	return reinterpret_cast<char*>(VirtualAlloc(NULL, _arenaSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
}
