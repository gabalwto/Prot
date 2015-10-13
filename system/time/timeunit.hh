#ifndef __PROT_LIBRARY_SYSTEM_TIMEUNIT_H_
#define __PROT_LIBRARY_SYSTEM_TIMEUNIT_H_

#include "defines.hh"
    
namespace Prot {
namespace Time {

	/** timeunit is in unit of 100 nanoseconds that could represent 29247 years max */

	class UNIT_TIME
	{
	public:
		inline UNIT_TIME(Int64 t) { _uts = t; }
	private:
		Int64 _uts;
		friend class TimeUnit;
	};

	class TimeUnit
	{
	public:
		//static TimeStamp FromMillisecond(Long);
		inline static TimeUnit FromNanoseconds(Int64 nanosecs) { return TimeUnit(nanosecs / HAS_NONOSECONDS); }
		inline static TimeUnit FromMacroseconds(Int64 macrosecs) { return TimeUnit(macrosecs * ONE_MACROSECOND); }
		inline static TimeUnit FromMilliseconds(Int64 millisecs) { return TimeUnit(millisecs * ONE_MILLISECOND); }
		inline static TimeUnit FromSeconds(Int64 seconds) { return TimeUnit(seconds * ONE_SECOND); }
		inline static TimeUnit FromSecondsFloat(Float seconds) { return TimeUnit((Int64)(seconds * ONE_SECOND)); }
		inline static TimeUnit Infinite() { return TimeUnit(INFINITE_UNITS); }
		inline static TimeUnit Zero() { return TimeUnit(0); }
	public:
		inline TimeUnit(UNIT_TIME ut){ _utc = ut._uts; }

		inline Int64 InMilliseconds() { return _utc / ONE_MILLISECOND; }
		inline Int64 InNanoseconds() { return _utc * HAS_NONOSECONDS; }
		inline Int64 InMacroseconds() { return _utc / ONE_MACROSECOND; }
		inline Int64 InSeconds() { return _utc / ONE_SECOND; }
		inline Float InFloatSeconds() { return _utc / (Float)ONE_SECOND; }

		inline Bool Infinitely() { return _utc == INFINITE_UNITS; }
		inline Bool Empty() { return _utc == 0; }
	public:
		const static Int32 HAS_NONOSECONDS = 100;
		const static Int32 ONE_MACROSECOND = 10;
		const static Int32 ONE_MILLISECOND = 10000;
		const static Int32 ONE_SECOND = 10000000;
		const static Int64 INFINITE_UNITS = 0x7fffffffffffffffll;
		const static Int64 MAXIMUM_UNITS = INFINITE_UNITS - 1;
	protected:
		Int64 _utc;		// Unit Tick Count
	};
}
}

#endif
