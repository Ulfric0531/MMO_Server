#include "pch.h"
#include "SpinLock.h"

void AcquireSpinLock(volatile long* lock)
{
	while (true)
	{
		if (InterlockedCompareExchange(lock, 1, 0) == 0)
		{
			break;
		}
		else
		{
			YieldProcessor();
		}
	}
}

void ReleaseSpinLock(volatile long* lock)
{
	InterlockedExchange(lock, 0);
}
