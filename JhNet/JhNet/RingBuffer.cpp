#include "pch.h"
#include "RingBuffer.h"

RingBuffer::RingBuffer(int bufferSize)
	: _front(0)
	, _rear(0)
	, _maxBufferSize(bufferSize + 1)
{
	_buffer = reinterpret_cast<char*>(PoolAllocator::Allocate(bufferSize + 1));
}

RingBuffer::~RingBuffer()
{
	PoolAllocator::Release(_buffer);
}

int RingBuffer::GetCurrentSize()
{
	if (_front <= _rear)
	{
		return _rear - _front;
	}
	return _maxBufferSize - _front + _rear;
}

int RingBuffer::GetFreeSize()
{
	if (_front <= _rear)
	{
		return (_maxBufferSize - 1) - (_rear - _front);
	}
	return _front - _rear - 1;
}

bool RingBuffer::Peek(char* elementDst, int elementSize)
{
	if (elementSize > GetCurrentSize())
	{
		return false;
	}

	if (DirectDequeueSize() >= elementSize)
	{
		memcpy(elementDst, _buffer + _front, elementSize);
		return true;
	}
	int dummyFront = _front;
	int directDequeueSize = DirectDequeueSize();

	memcpy(elementDst, _buffer + dummyFront, directDequeueSize);
	dummyFront = 0;
	memcpy(elementDst + directDequeueSize, _buffer + dummyFront, elementSize - directDequeueSize);
	return true;
}

void RingBuffer::ClearBuffer()
{
	_front = 0;
	_rear = 0;
}

char* RingBuffer::GetFrontPtr()
{
	return _buffer + _front;
}

char* RingBuffer::GetRearPtr()
{
	return _buffer + _rear;
}

int RingBuffer::DirectEnqueueSize()
{
	if (_front <= _rear)
	{
		return _maxBufferSize - _rear - 1;
	}

	return _front - _rear - 1;
}

int RingBuffer::DirectDequeueSize()
{
	if (_front <= _rear)
	{
		return GetCurrentSize();
	}

	return _maxBufferSize - _front - 1;
}

bool RingBuffer::Enqueue(char* elementSrc, int elementSize)
{
	if (GetFreeSize() < elementSize)
	{
		return false;
	}

	if (DirectEnqueueSize() >= elementSize)
	{
		memcpy(_buffer + _rear, elementSrc, elementSize);
		_rear = (_rear + elementSize) % _maxBufferSize;
		return true;
	}

	int directEnqueueSize = DirectEnqueueSize();
	memcpy(_buffer + _rear, elementSrc, directEnqueueSize);
	_rear = 0;
	memcpy(_buffer + _rear, elementSrc + directEnqueueSize, elementSize - directEnqueueSize);
	_rear = (_rear + elementSize - directEnqueueSize) % _maxBufferSize;
	return true;
}

bool RingBuffer::Dequeue(char* elementDst, int elementSize)
{
	if (elementSize > GetCurrentSize())
	{
		return false;
	}

	if (DirectDequeueSize() >= elementSize)
	{
		memcpy(elementDst, _buffer + _front, elementSize);
		_front = (_front + elementSize) % _maxBufferSize;
		return true;
	}

	int directDequeueSize = DirectDequeueSize();
	memcpy(elementDst, _buffer + _front, directDequeueSize);
	_front = 0;
	memcpy(elementDst + directDequeueSize, _buffer + _front, elementSize - directDequeueSize);
	_front = (_front + elementSize - directDequeueSize) % _maxBufferSize;
	return true;
}

bool RingBuffer::MoveFront(int elementSize)
{
	if (GetCurrentSize() <= 0)
	{
		return false;
	}

	if (GetCurrentSize() < elementSize)
	{
		return false;
	}

	_front = (_front + elementSize) % _maxBufferSize;

	return true;
}

bool RingBuffer::MoveRear(int elementSize)
{
	if (GetFreeSize() < elementSize)
	{
		return false;
	}

	_rear = (_rear + elementSize) % _maxBufferSize;

	return true;
}


