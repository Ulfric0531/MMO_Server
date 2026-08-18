#pragma once

class OverlappedEx;
class SendBuffer;
class RingBuffer;

class Session
{
public:
	Session(SOCKET socket, SOCKADDR_IN addr, unsigned long long id);
	~Session();

	SOCKET GetSockHandle();

	void IncreaseRefCount();
	void DecreaseRefCount();

	void RecvReserveTask();
	void SendReserveTask();
	void DisconnectReserveTask();

	void RecvCompletionTask(unsigned int completedBytes);
	void SendCompletionTask(unsigned int completedBytes);
	void DisconnectCompletionTask();

private:
	unsigned long long	_id;
	SendBuffer*			_sendPendingList;
	SOCKET				_socket;
	SOCKADDR_IN			_addr;
	RingBuffer			_recvBuffer;
	OverlappedEx		_recvOverlap;
	OverlappedEx		_sendOverlap;
	OverlappedEx		_disconnectOverlap;

	__declspec(align(64))volatile long _isConnected;
	__declspec(align(64))volatile long _refCount;

	
};

