#include "pch.h"
#include "RingBuffer.h"

RingBuffer::RingBuffer()
	: _front(0)
	, _rear(0)
	, _currentBufferSize(0)
	, _maxBufferSize(1024)
{
	_buffer = new char[1024];
}

RingBuffer::RingBuffer(int bufferSize)
	: _front(0)
	, _rear(0)
	, _currentBufferSize(0)
	, _maxBufferSize(bufferSize)
{
	_buffer = new char[bufferSize];
}

RingBuffer::~RingBuffer()
{

}

int RingBuffer::GetBufferSize()
{
	return _currentBufferSize;
}

int RingBuffer::GetFreeSize()
{
	return _maxBufferSize - _currentBufferSize;
}

bool RingBuffer::Peek(char* elementDst, int elementSize)
{
	if (elementSize > _currentBufferSize)
	{
		return false;
	}
}

void RingBuffer::ClearBuffer()
{
	_currentBufferSize = 0;
}

char* RingBuffer::GetBufferPtr()
{
	return _buffer;
}

char* RingBuffer::GetRearPtr()
{
	return _buffer + _rear;
}

int RingBuffer::DirectEnqueueSize()
{
	if (_rear >= _front)
	{
		return _maxBufferSize - _rear;
	}

	return GetFreeSize();
}

int RingBuffer::DirectDequeueSize()
{
	if (_rear >= _front)
	{
		return _currentBufferSize;
	}

	return _maxBufferSize - _front;
}

void RingBuffer::SetNextNode(RingBuffer* next)
{
	_next = next;
}

RingBuffer* RingBuffer::GetNextNode()
{
	return _next;
}

bool RingBuffer::MoveFront(int elementSize)
{
	if (_currentBufferSize <= 0)
	{
		return false;
	}

	if (_currentBufferSize < elementSize)
	{
		return false;
	}

	_front = (_front + elementSize) % _maxBufferSize;
	_currentBufferSize -= elementSize;

	return true;
}

bool RingBuffer::MoveRear(int elementSize)
{
	if (GetFreeSize() < elementSize)
	{
		return false;
	}

	_rear = (_rear + elementSize) % _maxBufferSize;
	_currentBufferSize += elementSize;

	return true;
}


