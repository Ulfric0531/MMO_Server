#pragma once

class RingBuffer
{
public:
	RingBuffer();
	RingBuffer(int bufferSize);
	~RingBuffer();

	int		GetBufferSize();
	int		GetFreeSize();
	void	ClearBuffer();
	char*	GetBufferPtr();
	char*	GetRearPtr();

	bool	Peek(char* elementDst, int elementSize);
	bool	MoveFront(int elementSize);
	bool	MoveRear(int elementSize);
	int		DirectEnqueueSize(void);
	int		DirectDequeueSize(void);


private:
	char*	_buffer;
	int		_front;
	int		_rear;
	int		_currentBufferSize;
	int		_maxBufferSize;
};

