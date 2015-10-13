#ifndef __PROT_LIBRARY_SYSTEM_TIMEDELTA_H_
#define __PROT_LIBRARY_SYSTEM_TIMEDELTA_H_

#include "timeunit.hh"
#include "timestamp.hh"
    
namespace Prot {
namespace Time {

	/** timedelta is in unit of 100 nanoseconds that could represent 29247 years max */
	class TimeDelta: public TimeUnit
	{
	public:
		inline TimeStamp AsTimeStamp() { return TimeStamp(_utc); }
	//	//static TimeStamp FromMillisecond(Long);
	//	inline static TimeDelta FromMilliseconds(Int64 millisecs) { return TimeDelta(millisecs * 10000); }
	//	inline static TimeDelta FromNanoseconds(Int64 nanosecs) { return TimeDelta(nanosecs / 100); }
	//	inline static TimeDelta FromMacroseconds(Int64 macrosecs) { return TimeDelta(macrosecs * 10); }
	//	inline static TimeDelta FromSeconds(Int64 seconds) { return TimeDelta(seconds * 10000000); }
	//	inline static TimeDelta FromFloatSeconds(Float seconds) { return TimeDelta(seconds * 10000000); }
	//	inline static TimeDelta Infinite() { return TimeDelta(0x7fffffffffffffffll); }
	//	inline static TimeDelta Zero() { return TimeDelta(0); }
	//public:
	//	inline TimeDelta(Int64 delta){ _delta = delta; }

	//	inline Int64 InMilliseconds() { return Infinitely() ? _delta : _delta / 10000; }
	//	inline Int64 InNanoseconds() { return Infinitely() ? _delta : _delta * 100; }
	//	inline Int64 InMacroseconds() { return Infinitely() ? _delta : _delta / 10; }
	//	inline Int64 InSeconds() { return Infinitely() ? _delta : _delta / 10000000; }
	//	inline Float ToSeconds() { return Infinitely() ? _delta : _delta / (Float)10000000; }

	//	inline Bool Infinitely() { return _delta == 0x7fffffffffffffffll; }
	//	inline Bool Empty() { return _delta == 0; }
	//private:
	//	Int64 _delta;
	};
}
}

#endif
