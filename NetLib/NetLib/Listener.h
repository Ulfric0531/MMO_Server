#pragma once

enum IoType_t;
class OverlappedEx;

#define PORT (7000)
class Listener
{
public:
	Listener();
	~Listener();

	void InitListener();
	void Dispatch();
	void RegisterAccept();
	void AcceptTask();

	IoType_t GetIoType();

private:
	SOCKET			_listenSock;
	SOCKADDR_IN		_serverAddr;
	HANDLE			_acceptIocp;
	OverlappedEx	_acceptOverlapped;
	OverlappedEx	_disconnectOverlapped;
};

