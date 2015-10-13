#ifndef __PROT_LIBRARY_SYSTEM_LOCK_CONDVAR_H_
#define __PROT_LIBRARY_SYSTEM_LOCK_CONDVAR_H_

#include "defines.hh"
#include "lock.hh"
#include <system/time/timedelta.hh>

namespace Prot {
namespace Lock {

	class ConditionVariable : public ILock
	{
	public:
		virtual LockType Type();
		virtual Bool Get();
		virtual Void Free();
		virtual Bool TryGet();
		/** lock will be free when condition variable is got
			lock object could be Null, in this case no lock
			be wake up when condition variable got.		*/
		ConditionVariable(ILock* lock);
		~ConditionVariable();
		Bool Wait(Time::TimeUnit t);
		Int32 WakeOne();
		Int32 WakeAll();
	private:
		AnyPtr _cvo;
	};

}
}


#endif
