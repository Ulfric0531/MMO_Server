#include "pch.h"

Session::Session()
{

}

Session::~Session()
{

}

SOCKET Session::GetSockHandle()
{
	return _socket;
}

void Session::RecvRegistrationTask()
{

}

void Session::SendRegistrationTask()
{

}

void Session::RecvCompletionTask(unsigned int completedBytes)
{
	int errCode;
	int retVal;

	RecvRegistrationTask();
}

void Session::SendCompletionTask(unsigned int completedBytes)
{
	int errCode;
	int retVal;

}
