#pragma once

enum IoType_t
{
	IOTYPE_RECV,
	IOTYPE_SEND,
	IOTYPE_ACCEPT,
	IOTYPE_DISCONNECT
};

class OverlappedEx : public OVERLAPPED
{
public:
	OverlappedEx(IoType_t ioType);
	~OverlappedEx();

	IoType_t	GetIoType();

private:
	IoType_t		_ioType;
};

