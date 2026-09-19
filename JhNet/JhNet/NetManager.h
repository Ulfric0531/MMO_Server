#pragma once

#define MAX_SESSION_COUNT (10000)

class NetManager
{
public:
	NetManager(WCHAR* serverIp, unsigned short serverPort, unsigned int threadCount);

	int			AddSession(Session* session);
	void		TryDeleteSession(unsigned int sessionIndex, unsigned long long sessionId);
	void		Send(SendBuffer* buffer, unsigned int size, unsigned int sessionIndex, unsigned long long sessionId);

private:
	SOCKET			_listenSock;
	SOCKADDR_IN		_serverAddr;
	OverlappedEx	_acceptOverlapped;
	OverlappedEx	_disconnectOverlapped;
	Iocp			_mainIocp;				// if client mode, recommend 1 thread.
	Iocp			_acceptDisconnectIocp;	// only for server mode. recommend 1 thread. 
private:
	Session*		_sessionList[MAX_SESSION_COUNT];
	SRWLOCK			_sessionListLock;
	unsigned int	_maxSessionCount;
	unsigned int	_currentSessionCount;
};

