#pragma once

#include "RingBuffer.h"
#include <atomic>
class OverlappedEx;
class SendBuffer;

using namespace std;

class Session
{
public:
	Session(SOCKET socket, SOCKADDR_IN addr, unsigned long long id);
	~Session();

	SOCKET GetSockHandle();

	void IncreaseRefCount();
	void DecreaseRefCount();

	void RecvPost();
	void TrySendPost(char* buffer, unsigned int size);
	void SendPost();
	void DisconnectPost();

	void RecvComplete(unsigned int completedBytes);
	void SendComplete(unsigned int completedBytes);
	void DisconnectComplete();

	unsigned long long GetId();
private:
	SRWLOCK					_sendBufferLock;
	unsigned long long		_id;
	SOCKET					_socket;
	SOCKADDR_IN				_addr;
	RingBuffer				_recvBuffer;
	RingBuffer				_sendBuffer;
	OverlappedEx			_recvOverlap;
	OverlappedEx			_sendOverlap;
	OverlappedEx			_disconnectOverlap;
	unsigned int			_index;
	volatile long			_refCount;
	volatile long			_isConnected;
	volatile long			_onSend;
private:
	char*			_sendPendingListHead;
	char*			_sendPendingListTail;
	unsigned int	_pendingListCount;
	SRWLOCK			_pendingListLock;

};