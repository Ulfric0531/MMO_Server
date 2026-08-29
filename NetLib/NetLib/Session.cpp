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
	_refCount.fetch_add(1);
}

void Session::DecreaseRefCount()
{
	_refCount.fetch_sub(1);
}

void Session::RecvReserveProc()
{
	if (_isConnected.load(memory_order_relaxed) == false)
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

			DisconnectReserveProc();
		}
	}
}

void Session::SendReserveProc(SendBuffer* sendBuffer)
{
	if (_isConnected.load(memory_order_relaxed) == false)
	{
		return;
	}

	
	if (_onSend.load(memory_order_relaxed) == true)
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
	
	IncreaseRefCount();

	int errCode = 0;
	int retVal = 0;
	unsigned long numOfBytes = 0;
	unsigned long flags = 0;

	_sendOverlap.Init();

	AcquireSRWLockExclusive(&_pendingListLock);
	unsigned long count = _pendingListCount;

	_sendOverlap._onFlightList = _sendPendingListHead;
	_sendPendingListHead = nullptr;
	_sendPendingListTail = nullptr;
	ReleaseSRWLockExclusive(&_pendingListLock);

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

			DisconnectReserveProc();
		}
	}
}

void Session::DisconnectReserveProc()
{
	if (_isConnected.exchange(false) == false)
	{
		return;
	}

	int errCode = 0;
	int retVal = 0;
	unsigned long numOfBytes = 0;
	unsigned long flags = 0;

	retVal = WinSockEx::DisconnectEx(_socket, reinterpret_cast<LPOVERLAPPED>(&_disconnectOverlap), flags, 0);
}

void Session::RecvCompletionProc(unsigned int completedBytes)
{


	RecvReserveProc();
}

void Session::SendCompletionProc(unsigned int completedBytes)
{
	int errCode;
	int retVal;

}

void Session::DisconnectCompletionProc()
{

}
