#include "tickcount.hh"

#if __PLATFORM_OSYS_ == PTOS_WINDOWS
#include <windows.h>
#else
#include <time.h>
#endif

namespace Prot {
namespace Time {

	UInt64 TickCount::NowMilliseconds()
	{
#if __PLATFORM_OSYS_ == PTOS_WINDOWS
		typedef ULONGLONG (WINAPI * LPGETTICKCOUNT64)();
		static HMODULE hKernel = NULL; 
		static LPGETTICKCOUNT64 lpGetTickCount64 = NULL;
		if (hKernel == NULL && lpGetTickCount64 == NULL) {
			hKernel = GetModuleHandleA("kernel32.dll");
			lpGetTickCount64 = (LPGETTICKCOUNT64)GetProcAddress(hKernel, "GetTickCount64");
		}
		if (lpGetTickCount64)
			return lpGetTickCount64();
		return GetTickCount();
#else
		struct timespec ts;
		if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
		{
			return ts.tv_sec*(UInt64)1000 + ts.tv_nsec / 1000000;
		}
//		printf("get timestamp failed !\n");
		((void(*)(void))0)();
		return 0;
#endif
	}

}
}
