#include "pch.h"
#include "OverlappedEx.h"
#include "SendBuffer.h"

#define DEFAULT_RECVBUFF_SIZE (4000)
#define DEFAULT_SENDBUFF_SIZE (16000)

Session::Session(SOCKET socket, SOCKADDR_IN addr, unsigned long long id)
	: _socket(socket)
	, _addr(addr)
	, _id(id)
	, _isConnected(true)
	, _refCount(1)
	, _recvOverlap(OverlappedEx(IOTYPE_RECV))
	, _sendOverlap(OverlappedEx(IOTYPE_SEND))
	, _disconnectOverlap(OverlappedEx(IOTYPE_DISCONNECT))
	, _emergencyBufferHead(nullptr)
	, _emergencyBufferTail(nullptr)
	, _recvBuffer(RingBuffer(DEFAULT_RECVBUFF_SIZE))
	, _sendBuffer(RingBuffer(DEFAULT_SENDBUFF_SIZE))
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
	if (InterlockedDecrement(&_refCount) == 1)
	{
		// NetManager에서 ReleaseSession 처리
	}
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
	wsabuf[1].buf = _recvBuffer.GetFrontPtr();
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
	bool enqueueResult;

	if (InterlockedCompareExchange(&_isConnected, 0, 0) == 0)
	{
		return;
	}

	{
		AcquireSRWLockExclusive(&_sendBufferLock);
		enqueueResult = _sendBuffer.Enqueue(buffer, size);
		ReleaseSRWLockExclusive(&_sendBufferLock);
	}

	if (enqueueResult == false)
	{
		goto BufferIsFull;
	}

	if (InterlockedCompareExchange(&_onSend, 1, 0) == 1)
	{
		return;
	}

	SendPost();
	return;

BufferIsFull:
	EmergencyBuffer* sendBuffer = reinterpret_cast<EmergencyBuffer*>(PoolAllocator::Allocate(sizeof(EmergencyBuffer)));
	sendBuffer->buffer = buffer;
	sendBuffer->size = size;
	sendBuffer->next = nullptr;

	{
		AcquireSRWLockExclusive(&_emergencyBufferLock);
		if (_emergencyBufferHead == nullptr)
		{
			_emergencyBufferHead = sendBuffer;
			_emergencyBufferTail = sendBuffer;
			goto Exit;
		}
		_emergencyBufferTail->next = sendBuffer;
		_emergencyBufferTail = sendBuffer;
Exit:
		ReleaseSRWLockExclusive(&_emergencyBufferLock);
	}
}

void Session::SendPost()
{
	if (InterlockedCompareExchange(&_isConnected, 0, 0) == 0)
	{
		return;
	}

	IncreaseRefCount();

	int errCode = 0;
	int retVal = 0;
	int idx = 0;
	int directDequeueSize;
	int remainSize;
	unsigned long numOfBytes = 0;
	unsigned long flags = 0;
	unsigned int count = 0;
	_sendOverlap.Init();

	{
		AcquireSRWLockExclusive(&_emergencyBufferLock);

		if (_emergencyBufferHead != nullptr)
		{
			count = _emergencyBufferCount;

			_sendOverlap._onFlightList = _emergencyBufferHead;
			_emergencyBufferHead = nullptr;
			_emergencyBufferTail = nullptr;
			_emergencyBufferCount = 0;
		}

		ReleaseSRWLockExclusive(&_emergencyBufferLock);
	}

	WSABUF* wsabuf = reinterpret_cast<WSABUF*>(PoolAllocator::Allocate(sizeof(WSABUF) * count + 2));
	EmergencyBuffer* onFlightBuffer = _sendOverlap._onFlightList;
	while (onFlightBuffer != nullptr)
	{
		wsabuf[idx].buf = onFlightBuffer->buffer;
		wsabuf[idx].len = onFlightBuffer->size;
		onFlightBuffer = onFlightBuffer->next;
	}

	directDequeueSize = _sendBuffer.DirectDequeueSize();
	remainSize = _sendBuffer.GetCurrentSize() - directDequeueSize;
	wsabuf[idx].buf = _sendBuffer.GetFrontPtr();
	wsabuf[idx].len = directDequeueSize;
	_sendBuffer.MoveFront(directDequeueSize);
	wsabuf[idx + 1].buf = _sendBuffer.GetFrontPtr();
	wsabuf[idx + 1].len = remainSize;

	retVal = WSASend(_socket, wsabuf, count + 2, &numOfBytes, flags, reinterpret_cast<LPOVERLAPPED>(&_sendOverlap), nullptr);
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

	IncreaseRefCount();

	int errCode = 0;
	int retVal = 0;
	unsigned long numOfBytes = 0;
	unsigned long flags = 0;

	retVal = WinSockEx::DisconnectEx(_socket, reinterpret_cast<LPOVERLAPPED>(&_disconnectOverlap), flags, 0);
	if (retVal == SOCKET_ERROR)
	{
		errCode = WSAGetLastError();
		if (errCode != WSA_IO_PENDING)
		{
			DecreaseRefCount();
		}
	}
}

void Session::RecvComplete(unsigned int completedBytes)
{
	if (InterlockedCompareExchange(&_isConnected, 0, 0) == 0)
	{
		return;
	}

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