#pragma once



#include "Allocator.h"

#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <process.h>
#pragma comment(lib, "ws2_32.lib")

#include "Overloading.h"
#include "Session.h"
#include "Listener.h"
#include "OverlappedEx.h"
#include "Iocp.h"
#include "RingBuffer.h"


using namespace std;

namespace WinSockEx
{
	extern LPFN_ACCEPTEX		AcceptEx;
	extern LPFN_CONNECTEX		ConnectEx;
	extern LPFN_DISCONNECTEX	DisconnectEx;
}
