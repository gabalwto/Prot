#include "sysinfo.hh"

#if (__PLATFORM_OSYS_ == PTOS_WINDOWS)
#include <windows.h>
#else
#include <sys/sysinfo.h>
#endif

namespace Prot {
namespace System {

#if (__PLATFORM_OSYS_ == PTOS_WINDOWS)
	Int32 Information::NumOfCPUCores()
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		return si.dwNumberOfProcessors;
	}
#else
	Int32 Information::NumOfCPUCores()
	{
		return get_nprocs();
	}
#endif

}
}
