#include "pch.h"
#include "OverlappedEx.h"
#include "SendBuffer.h"

Session::Session(SOCKET socket, SOCKADDR_IN addr, unsigned long long id)
	: _socket(socket)
	, _addr(addr)
	, _id(0)
	, _isConnected(true)
	, _refCount(1)
	, _recvOverlap(OverlappedEx(IOTYPE_RECV))
	, _sendOverlap(OverlappedEx(IOTYPE_SEND))
	, _disconnectOverlap(OverlappedEx(IOTYPE_DISCONNECT))
	, _sendPendingListHead(nullptr)
	, _sendPendingListTail(nullptr)
	, _recvBuffer(RingBuffer(4096))
	, _sendBuffer(RingBuffer(16000))
{

}

Session::~Session()
{
	
}

SOCKET Session::GetSockHandle()
{
	return _socket;
}

void Session::IncreaseRefCount()
{
	InterlockedIncrement(&_refCount);
}

void Session::DecreaseRefCount()
{
	InterlockedDecrement(&_refCount);
}

void Session::RecvPost()
{
	if (InterlockedCompareExchange(&_isConnected, 0, 0) == 0)
	{
		return;
	}

	IncreaseRefCount();

	int errCode = 0;
	int retVal = 0;
	unsigned long numOfBytes = 0;
	unsigned long flags = 0;

	_recvOverlap.Init();

	int directEnqueueSize = _recvBuffer.DirectEnqueueSize();
	int remainSize = _recvBuffer.GetFreeSize() - directEnqueueSize;
	WSABUF wsabuf[2];
	wsabuf[0].buf = _recvBuffer.GetRearPtr();
	wsabuf[0].len = directEnqueueSize;
	wsabuf[1].buf = _recvBuffer.GetBufferPtr();
	wsabuf[1].len = remainSize;

	retVal = WSARecv(_socket, wsabuf, 2, &numOfBytes, &flags, reinterpret_cast<LPOVERLAPPED>(&_recvOverlap), nullptr);
	if (retVal == SOCKET_ERROR)
	{
		errCode = WSAGetLastError();
		if (errCode != WSA_IO_PENDING)
		{
			DecreaseRefCount();

			DisconnectPost();
		}
	}
}

void Session::TrySendPost(char* buffer, unsigned int size)
{
	if (InterlockedCompareExchange(&_isConnected, 0, 0) == 0)
	{
		return;
	}

	int freeSize;

	{
		AcquireSRWLockExclusive(&_sendBufferLock);
		freeSize = _sendBuffer.GetFreeSize();
		if (freeSize < size)
		{
			ReleaseSRWLockExclusive(&_sendBufferLock);
			goto BufferIsFull;
		}

		_sendBuffer.Enqueue(buffer, size);
		ReleaseSRWLockExclusive(&_sendBufferLock);
	}

	goto TryPost;

BufferIsFull:
	{
		AcquireSRWLockExclusive(&_pendingListLock);
		if (_sendPendingListHead == nullptr)
		{
			_sendPendingListHead = buffer;
			_sendPendingListTail = buffer;

			ReleaseSRWLockExclusive(&_pendingListLock);
			return;
		}
		_sendPendingListTail->SetNextNode(sendBuffer);
		_sendPendingListTail = sendBuffer;
		ReleaseSRWLockExclusive(&_pendingListLock);
	}
	return;

TryPost:
	if (InterlockedCompareExchange(&_onSend, 1, 0) == 1)
	{
		return;
	}

	SendPost();
Exit:
}

void Session::SendPost()
{
	if (InterlockedCompareExchange(&_isConnected, 0, 0) == 0)
	{
		return;
	}


	/*
	if (InterlockedCompareExchange(&_onSend, 1, 0) == 1)
	{
		AcquireSRWLockExclusive(&_pendingListLock);
		if (_sendPendingListHead == nullptr)
		{
			_sendPendingListHead = sendBuffer;
			_sendPendingListTail = sendBuffer;

			ReleaseSRWLockExclusive(&_pendingListLock);
			return;
		}
		_sendPendingListTail->SetNextNode(sendBuffer);
		_sendPendingListTail = sendBuffer;
		ReleaseSRWLockExclusive(&_pendingListLock);
		return;
	}
	*/
	IncreaseRefCount();

	int errCode = 0;
	int retVal = 0;
	unsigned long numOfBytes = 0;
	unsigned long flags = 0;
	unsigned int count = 0;
	_sendOverlap.Init();

	{
		AcquireSRWLockExclusive(&_pendingListLock);
		count = _pendingListCount;

		_sendOverlap._onFlightList = _sendPendingListHead;
		_sendPendingListHead = nullptr;
		_sendPendingListTail = nullptr;

		ReleaseSRWLockExclusive(&_pendingListLock);
	}

	WSABUF* wsabuf = reinterpret_cast<WSABUF*>(PoolAllocator::Allocate(sizeof(WSABUF) * count));
	SendBuffer* onFlightBuffer = _sendOverlap._onFlightList;
	for (int i = 0; i < count; ++i)
	{
		wsabuf[i].buf = onFlightBuffer->GetBufferPtr();
		wsabuf[i].len = onFlightBuffer->GetCurrentSize();
		onFlightBuffer = _sendOverlap._onFlightList->GetNextNode();
	}

	retVal = WSASend(_socket, wsabuf, count, &numOfBytes, flags, reinterpret_cast<LPOVERLAPPED>(&_sendOverlap), nullptr);
	PoolAllocator::Release(wsabuf);

	if (retVal == SOCKET_ERROR)
	{
		errCode = WSAGetLastError();
		if (errCode != WSA_IO_PENDING)
		{
			DecreaseRefCount();

			DisconnectPost();
		}
	}
}

void Session::DisconnectPost()
{
	if (InterlockedExchange(&_isConnected, 0) == 0)
	{
		return;
	}

	int errCode = 0;
	int retVal = 0;
	unsigned long numOfBytes = 0;
	unsigned long flags = 0;

	retVal = WinSockEx::DisconnectEx(_socket, reinterpret_cast<LPOVERLAPPED>(&_disconnectOverlap), flags, 0);
}

void Session::RecvComplete(unsigned int completedBytes)
{


	RecvPost();
}

void Session::SendComplete(unsigned int completedBytes)
{
	int errCode;
	int retVal;

}

void Session::DisconnectComplete()
{

}

unsigned long long Session::GetId()
{
	return _id;
}