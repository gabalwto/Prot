#include "timestamp.hh"

#if __PLATFORM_OSYS_ == PTOS_WINDOWS
#include <windows.h>
/* 1601*365*24*3600*10000000 = 504891360000000000 */
#define PT_BASE_TS_UTC_PREFIX 504891360000000000LL
#else
#include <sys/time.h>
/* 1970*365*24*3600*10000000 = 621259200000000000 */
#define PT_BASE_TS_UTC_PREFIX 621259200000000000LL
#endif

namespace Prot {
namespace Time {


	Int64 TimeStamp::NowMilliseconds()
	{
#if __PLATFORM_OSYS_ == PTOS_WINDOWS
		FILETIME ft;
		GetSystemTimeAsFileTime(&ft);
		return TimeUnit(((Int64)ft.dwHighDateTime << 32) + ft.dwLowDateTime
						+ PT_BASE_TS_UTC_PREFIX).InMilliseconds();
#else
		struct timeval tv;
		if (gettimeofday(&tv, 0) == 0)
		{
			return tv.tv_sec*(Int64)1000 + tv.tv_usec / 1000 + 
				PT_BASE_TS_UTC_PREFIX / ONE_MILLISECOND;
			//return TimeUnit::FromMacroseconds(tv.tv_sec*1000000+tv.tv_usec).InMilliseconds();
		}
		// printf("get timestamp failed !\n");
		((void(*)(void))0)();
		return 0;
#endif
	}

}
}
