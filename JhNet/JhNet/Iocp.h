#pragma once
class Session;
class Listener;

class Iocp
{
public:
	Iocp() = delete;
	Iocp(unsigned int threadCount);
	~Iocp();

	void DispatchIocpQueue();

private:
	HANDLE _iocpHandle;
};

