#ifndef __PROT_LIBRARY_SYSTEM_TIMESTAMP_H_
#define __PROT_LIBRARY_SYSTEM_TIMESTAMP_H_

#include "timeunit.hh"
    
namespace Prot {
namespace Time {

	/*** the timestamp value is related to (00:00:00 UTC, January 1, 0000) **/

	class TimeStamp:public TimeUnit
	{
	public:
		//static TimeStamp FromMillisecond(Long);
		static Int64 NowMilliseconds();
	public:
		TimeStamp(Int64 uts);
	};
}
}

#endif
