#include "pch.h"
#include "SendBuffer.h"

#define DEFAULT_SENDBUF_SIZE (8192)

//---------------------
//		SendBuffer
//---------------------

SendBuffer::SendBuffer()
	: _front(0)
	, _rear(0)
	, _maxBufferSize(DEFAULT_SENDBUF_SIZE)
	, _buffer(nullptr)
	, _owner(nullptr)
	, _next(nullptr)
{

}

SendBuffer::SendBuffer(unsigned int bufferSize)
	: _front(0)
	, _rear(0)
	, _maxBufferSize(bufferSize)
	, _buffer(nullptr)
	, _owner(nullptr)
	, _next(nullptr)
{

}

SendBuffer::~SendBuffer()
{

}

char* SendBuffer::GetBufferPtr()
{
	return _buffer + _rear;
}

bool SendBuffer::Write(void* srcPtr, unsigned int size)
{
	return false;
}

bool SendBuffer::MoveRearPos(unsigned int size)
{
	return false;
}

unsigned int SendBuffer::GetFreeSize()
{
	return unsigned int();
}

unsigned int SendBuffer::GetCurrentSize()
{
	return unsigned int();
}

void SendBuffer::SetNextNode(SendBuffer* next)
{
	_next = next;
}

SendBuffer* SendBuffer::GetNextNode()
{
	return _next;
}
