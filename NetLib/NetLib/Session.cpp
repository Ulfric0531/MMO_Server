#include "pch.h"

Session::Session(SOCKET socket, SOCKADDR_IN addr, unsigned long long id)
	: _socket(socket)
	, _addr(addr)
	, _id(0)
	, _isConnected(1)
	, _refCount(1)
	, _recvOverlap(OverlappedEx(IOTYPE_RECV))
	, _sendOverlap(OverlappedEx(IOTYPE_SEND))
	, _disconnectOverlap(OverlappedEx(IOTYPE_DISCONNECT))
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

void Session::RecvReserveTask()
{

}

void Session::SendReserveTask()
{

}

void Session::DisconnectReserveTask()
{
	if (InterlockedExchange(&_isConnected, 1) == 1)
	{
		return;
	}
}

void Session::RecvCompletionTask(unsigned int completedBytes)
{
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
			DisconnectReserveTask();
		}
	}

	RecvReserveTask();
}

void Session::SendCompletionTask(unsigned int completedBytes)
{
	int errCode;
	int retVal;

}

void Session::DisconnectCompletionTask()
{

}
