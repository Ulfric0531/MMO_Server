#include "pch.h"
#include "NetAddr.h"

NetAddr::NetAddr()
{
	_addr = { 0, };
}

NetAddr::NetAddr(const WCHAR* ip, unsigned short port)
{
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);
	InetPtonW(AF_INET, ip, &_addr.sin_addr);
}

SOCKADDR_IN* NetAddr::GetSockAddr()
{
	return &_addr;
}

void NetAddr::SetSockAddr(const WCHAR* ip, unsigned short port)
{
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);
	InetPtonW(AF_INET, ip, &_addr.sin_addr);
}

void NetAddr::GetIp(IN_ADDR ip, WCHAR* outBuf, unsigned int bufSize)
{
	InetNtopW(AF_INET, &_addr.sin_addr, outBuf, bufSize);
}

unsigned short NetAddr::GetPort()
{
	return ntohs(_addr.sin_port);
}
