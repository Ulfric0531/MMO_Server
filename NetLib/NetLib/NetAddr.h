#pragma once
class NetAddr
{
public:
	NetAddr();
	NetAddr(const WCHAR* ip, unsigned short port);

	SOCKADDR_IN*	GetSockAddr();
	void			SetSockAddr(const WCHAR* ip, unsigned short port);
	void			GetIp(IN_ADDR ip, WCHAR* outBuf, unsigned int bufSize);
	unsigned short	GetPort();

private:
	SOCKADDR_IN _addr;

};

