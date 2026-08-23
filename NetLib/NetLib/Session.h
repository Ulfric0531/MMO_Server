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
	void SendReserveTask(SendBuffer* sendBuffer);
	void DisconnectReserveTask();

	void RecvCompletionTask(unsigned int completedBytes);
	void SendCompletionTask(unsigned int completedBytes);
	void DisconnectCompletionTask();

private:
	unsigned long long		_id;
	atomic<unsigned int>	_refCount;
	atomic<bool>			_isConnected;
	atomic<bool>			_onSend;
	SOCKET					_socket;
	SOCKADDR_IN				_addr;
	RingBuffer				_recvBuffer;
	OverlappedEx			_recvOverlap;
	OverlappedEx			_sendOverlap;
	OverlappedEx			_disconnectOverlap;
private:
	SendBuffer* _sendPendingListHead;
	SendBuffer* _sendPendingListTail;
	SRWLOCK		_pendingListLock;

};

