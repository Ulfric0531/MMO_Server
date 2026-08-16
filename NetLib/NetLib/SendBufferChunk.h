#pragma once
class SendBuffer;

class SendBufferChunk
{
public:
	SendBufferChunk();
	SendBufferChunk(unsigned int chunkSize);
	~SendBufferChunk();

	void			Clear();
	void			DisableChunk();
	char*			GetSendBuffer(unsigned int size);
	void			IncrementRefCount();
	void			DecrementRefCount();
	unsigned int	GetFreeSize();
	
private:
	char*			_chunk;
	unsigned int	_front;
	unsigned int	_rear;
	unsigned int	_maxChunkSize;
	volatile long	_refCount;

	__declspec(align(64))volatile long _isEnabled;
};

