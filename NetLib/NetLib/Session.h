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

	void RecvReserveProc();
	void SendReserveProc(SendBuffer* sendBuffer);
	void DisconnectReserveProc();

	void RecvCompletionProc(unsigned int completedBytes);
	void SendCompletionProc(unsigned int completedBytes);
	void DisconnectCompletionProc();

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
	SendBuffer*		_sendPendingListHead;
	SendBuffer*		_sendPendingListTail;
	unsigned int	_pendingListCount;
	SRWLOCK			_pendingListLock;

};

