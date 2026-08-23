#pragma once

class SendBufferChunk;

class SendBuffer
{
public:
	SendBuffer();
	SendBuffer(unsigned int bufferSize);
	~SendBuffer();

	char*	GetBufferPtr();
	bool	Write(void* srcPtr, unsigned int size);
	bool	MoveRearPos(unsigned int size);

	unsigned int	GetFreeSize();
	unsigned int	GetCurrentSize();
public:
	void		SetNextNode(SendBuffer* next);
	SendBuffer* GetNextNode();

private:
	char*				_buffer;
	SendBufferChunk*	_owner;
	unsigned int		_front;
	unsigned int		_rear;
	unsigned int		_maxBufferSize;
private:
	SendBuffer* _next; // for session send pending list.

};
