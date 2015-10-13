#ifndef __PROT_LIBRARY_SYSTEM_TICKCOUNT_H_
#define __PROT_LIBRARY_SYSTEM_TICKCOUNT_H_

#include "timeunit.hh"
    
namespace Prot {
namespace Time {
	class TickCount:public TimeUnit
	{
	public:
		//static TimeStamp FromMillisecond(Long);
		static UInt64 NowMilliseconds();
	public:
		TickCount();
	};
}
}

#endif
