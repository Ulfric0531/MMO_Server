#include "pch.h"
#include "SendBufferChunk.h"

#define DEFAULT_SENDBUFCHUNK_SIZE (1024 * 10)

SendBufferChunk::SendBufferChunk()
	: _chunk(nullptr)
	, _front(0)
	, _rear(0)
	, _maxChunkSize(DEFAULT_SENDBUFCHUNK_SIZE)
	, _refCount(0)
	, _isEnabled(1)
{
	// TODO : _chunk 받아오기
}

SendBufferChunk::SendBufferChunk(unsigned int chunkSize)
	: _chunk(nullptr)
	, _front(0)
	, _rear(0)
	, _maxChunkSize(chunkSize)
	, _refCount(0)
	, _isEnabled(1)
{
	// TODO : _chunk 받아오기
}

SendBufferChunk::~SendBufferChunk()
{

}

void SendBufferChunk::Clear()
{

}

void SendBufferChunk::DisableChunk()
{
	InterlockedExchange(&_isEnabled, 0);
}

char* SendBufferChunk::GetSendBuffer(unsigned int size)
{
	char* buffer = _chunk + _rear;
	_rear += size;
	return buffer;
}

void SendBufferChunk::IncrementRefCount()
{
	InterlockedIncrement(&_refCount);
}

void SendBufferChunk::DecrementRefCount()
{
	InterlockedDecrement(&_refCount);
}

unsigned int SendBufferChunk::GetFreeSize()
{
	return _maxChunkSize - _rear;
}
