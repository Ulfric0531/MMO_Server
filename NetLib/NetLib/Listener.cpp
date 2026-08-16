#include "pch.h"
#include "Listener.h"

LPFN_ACCEPTEX		WinSockEx::AcceptEx = nullptr;
LPFN_CONNECTEX		WinSockEx::ConnectEx = nullptr;
LPFN_DISCONNECTEX	WinSockEx::DisconnectEx = nullptr;

Listener::Listener()
	: _acceptOverlapped(OverlappedEx(IOTYPE_ACCEPT))
	, _disconnectOverlapped(OverlappedEx(IOTYPE_DISCONNECT))
{
	int bindRetval;
	int listenRetval;
	int lingerOptRetval;
	int reuseAddrOptRetval;
	int errCode;

	_acceptIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 1);
	_serverAddr.SetSockAddr(L"0.0.0.0", PORT);
	_listenSock = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	bindRetval = ::bind(_listenSock, reinterpret_cast<SOCKADDR*>(_serverAddr.GetSockAddr()), sizeof(_serverAddr.GetSockAddr()));
	if (bindRetval == SOCKET_ERROR)
	{
		errCode = ::WSAGetLastError();
		wcout << L"Listen Socket bind() error : " << errCode << endl;
	}

	listenRetval = ::listen(_listenSock, SOMAXCONN);
	if (listenRetval == SOCKET_ERROR)
	{
		errCode = ::WSAGetLastError();
		wcout << L"Listen Socket listen() error : " << errCode << endl;
	}

	linger lingerOption;
	lingerOption.l_linger = 0;
	lingerOption.l_onoff = 0;
	lingerOptRetval = setsockopt(_listenSock, SOL_SOCKET, SO_LINGER, reinterpret_cast<const char*>(&lingerOption), sizeof(linger));
	if (lingerOptRetval == SOCKET_ERROR)
	{
		errCode = ::WSAGetLastError();
		wcout << L"Listen Socket Linger setsockopt() error : " << errCode << endl;
	}

	bool reuseAddrOption = true;
	reuseAddrOptRetval = setsockopt(_listenSock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuseAddrOption), sizeof(bool));
	if (reuseAddrOptRetval == SOCKET_ERROR)
	{
		errCode = ::WSAGetLastError();
		wcout << L"Listen Socket Reuse Address setsockopt() error : " << errCode << endl;
	}
	
}

Listener::~Listener()
{

}

void Listener::InitListener()
{
	DWORD bytes;
	GUID acceptId = WSAID_ACCEPTEX;
	GUID connectId = WSAID_CONNECTEX;
	GUID disconnectId = WSAID_DISCONNECTEX;


	SOCKET dummy = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	::WSAIoctl(dummy, SIO_GET_EXTENSION_FUNCTION_POINTER, &acceptId, sizeof(acceptId), &WinSockEx::AcceptEx, sizeof(WinSockEx::AcceptEx), &bytes, NULL, NULL);
	::WSAIoctl(dummy, SIO_GET_EXTENSION_FUNCTION_POINTER, &connectId, sizeof(connectId), &WinSockEx::ConnectEx, sizeof(WinSockEx::ConnectEx), &bytes, NULL, NULL);
	::WSAIoctl(dummy, SIO_GET_EXTENSION_FUNCTION_POINTER, &disconnectId, sizeof(disconnectId), &WinSockEx::DisconnectEx, sizeof(WinSockEx::DisconnectEx), &bytes, NULL, NULL);
	::closesocket(dummy);

}

void Listener::Dispatch()
{
	DWORD numOfBytes = 0;
	Session* session = nullptr;
	OverlappedEx* ioEvent = nullptr;

	
	if (GetQueuedCompletionStatus(_acceptIocp, &numOfBytes, reinterpret_cast<PULONG_PTR>(session), reinterpret_cast<LPOVERLAPPED*>(ioEvent), 0))
	{
		if (ioEvent->GetIoType() != IOTYPE_ACCEPT)
		{
			// 에러 처리
		}

		// TODO
	}
}
