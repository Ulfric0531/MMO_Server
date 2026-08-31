#pragma once

#define MAX_SESSION_COUNT (10000)

class NetManager
{
public:
	NetManager(WCHAR* serverIp, unsigned short serverPort, unsigned int threadCount);

private:
	SOCKET			_listenSock;
	SOCKADDR_IN		_serverAddr;
	OverlappedEx	_acceptOverlapped;
	OverlappedEx	_disconnectOverlapped;
	Iocp			_mainIocp;
	Iocp			_acceptDisconnectIocp; // recommend 1 thread.
private:
	Session*		_sessionList[MAX_SESSION_COUNT];
	unsigned int	_maxSessionCount;
	unsigned int	_currentSessionCount;
};

