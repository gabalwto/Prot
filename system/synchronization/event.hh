#ifndef __PROT_LIBRARY_SYSTEM_LOCK_EVENT_H_
#define __PROT_LIBRARY_SYSTEM_LOCK_EVENT_H_

#include "defines.hh"
#include "lock.hh"
#include <system/time/timeunit.hh>

namespace Prot {
namespace Lock {

	typedef enum {
		Auto = 0,
		Manual = 1
	} EventType;

	/** Internal Event class for template use */
	class IEvent :public ILock
	{
	public:
		virtual LockType Type();
		virtual Bool Get();
		virtual Void Free();
		virtual Bool TryGet();
		/** Event has two type of signal state */
		Void Set();
		Void Reset();
		Bool Wait(Time::TimeUnit t);
		~IEvent();
	protected:
		IEvent(EventType event_type, Bool init_state);
		AnyPtr _eso;
	};

	template <EventType _Type>
	class Event :public IEvent
	{
	public:
		Event(Bool init_state)
			: IEvent(_Type, init_state){}
		virtual LockType Type() {
			if (!_eso)
				return Lock::TypeInvalid;
			return _Type == Auto ? TypeAutoResetEvent :TypeManualResetEvent;
		}
	};

	typedef Event<Auto> AutoResetEvent;
	typedef Event<Manual> ManualResetEvent;
	
}
}

#endif
