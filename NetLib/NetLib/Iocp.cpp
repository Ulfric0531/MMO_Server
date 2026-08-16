#include "Iocp.h"
#include "pch.h"

Iocp::Iocp(unsigned int threadCount)
{
	_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, threadCount);
}

Iocp::~Iocp()
{
	CloseHandle(_iocpHandle);
}

void Iocp::DispatchIocpQueue()
{
	unsigned long completedBytes = 0;
	Session* session = nullptr;
	OverlappedEx* ioEvent = nullptr;

	if (GetQueuedCompletionStatus(_iocpHandle, &completedBytes, reinterpret_cast<PULONG_PTR>(session), reinterpret_cast<LPOVERLAPPED*>(ioEvent), 0))
	{
		switch (ioEvent->GetIoType())
		{
		case IOTYPE_ACCEPT:
			break;
		case IOTYPE_DISCONNECT:
			break;
		case IOTYPE_RECV:
			session->RecvCompletionTask(static_cast<unsigned int>(completedBytes));
			break;
		case IOTYPE_SEND:
			session->SendCompletionTask(static_cast<unsigned int>(completedBytes));
			break;
		default:
			break;
		}
	}
}
