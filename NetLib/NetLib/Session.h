#pragma once
#include "OverlappedEx.h"
#include "RingBuffer.h"

class Session
{
public:
	Session();
	~Session();

	SOCKET GetSockHandle();

	void RecvRegistrationTask();
	void SendRegistrationTask();
	void RecvCompletionTask(unsigned int completedBytes);
	void SendCompletionTask(unsigned int completedBytes);

private:
	SOCKET		_socket;
	NetAddr		_addr;
	RingBuffer	_recvBuffer;
};

