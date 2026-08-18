#include "pch.h"

OverlappedEx::OverlappedEx(IoType_t ioType)
	: _ioType(ioType)
{
	memset(this, 0, sizeof(OVERLAPPED));
}

OverlappedEx::~OverlappedEx()
{}

IoType_t OverlappedEx::GetIoType()
{
	return _ioType;
}

void OverlappedEx::Init()
{
	// OVERLAPPED
	hEvent = 0;
	Offset = 0;
	OffsetHigh = 0;
	Internal = 0;
	InternalHigh = 0;
}
