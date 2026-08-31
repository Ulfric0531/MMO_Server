#include "pch.h"
#include "NetManager.h"

NetManager::NetManager(WCHAR* serverIp, unsigned short serverPort, unsigned int threadCount)
	: _acceptOverlapped(OverlappedEx(IOTYPE_ACCEPT))
	, _disconnectOverlapped(OverlappedEx(IOTYPE_DISCONNECT))
	, _mainIocp(Iocp(threadCount))
	, _acceptDisconnectIocp(Iocp(1))
{

//--------------------------
//	  Init Listen Socket
//--------------------------
	int bindRetval;
	int listenRetval;
	int lingerOptRetval;
	int reuseAddrOptRetval;
	int errCode;

	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(serverPort);
	InetPton(AF_INET, L"0.0.0.0", reinterpret_cast<SOCKADDR*>(&_serverAddr.sin_addr));
	_listenSock = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	bindRetval = ::bind(_listenSock, reinterpret_cast<SOCKADDR*>(&_serverAddr), sizeof(_serverAddr));
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
