#pragma once

#define MAX_SESSION_COUNT (10000)

class NetManager
{
public:
	NetManager(WCHAR* serverIp, unsigned short serverPort, unsigned int threadCount);

	void		AddSession(Session* session);
	void		TryDeleteSession(unsigned int index, unsigned int id);
	Session*	GetSessionOrNull(unsigned int index);

private:
	SOCKET			_listenSock;
	SOCKADDR_IN		_serverAddr;
	OverlappedEx	_acceptOverlapped;
	OverlappedEx	_disconnectOverlapped;
	Iocp			_mainIocp;				// if client mode, recommend 1 thread.
	Iocp			_acceptDisconnectIocp;	// only for server mode. recommend 1 thread. 
private:
	Session*		_sessionList[MAX_SESSION_COUNT];
	unsigned int	_maxSessionCount;
	unsigned int	_currentSessionCount;
};

