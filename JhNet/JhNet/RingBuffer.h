#pragma once

class RingBuffer
{
public:
	RingBuffer() = delete;
	RingBuffer(int bufferSize);
	~RingBuffer();

	int		GetCurrentSize();
	int		GetFreeSize();
	void	ClearBuffer();
	char*	GetFrontPtr();
	char*	GetRearPtr();

	bool	Enqueue(char* elementSrc, int elementSize);
	bool	Dequeue(char* elementDst, int elementSize);
	bool	Peek(char* elementDst, int elementSize);
	bool	MoveFront(int elementSize);
	bool	MoveRear(int elementSize);
	int		DirectEnqueueSize(void);
	int		DirectDequeueSize(void);
public:
	void		SetNextNode(RingBuffer* next);
	RingBuffer* GetNextNode();


private:
	char*	_buffer;
	int		_front;
	int		_rear;
	int		_maxBufferSize;
};

