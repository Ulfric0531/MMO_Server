#ifndef DLL_PROC
#define DLL_PROC extern "C" __declspec(dllimport)
#endif

DLL_PROC void*	Allocate(unsigned int allocSize);

DLL_PROC void	Release(void* ptr);