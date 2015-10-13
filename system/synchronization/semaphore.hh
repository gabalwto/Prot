#ifndef __PROT_LIBRARY_SYSTEM_LOCK_SEMAPHORE_H_
#define __PROT_LIBRARY_SYSTEM_LOCK_SEMAPHORE_H_

#include "defines.hh"
#include "lock.hh"
#include <system/time/timedelta.hh>

namespace Prot {
namespace Lock {

	class Semaphore : public ILock
	{
	public:
		virtual LockType Type();
		virtual Bool Get();
		virtual Void Free();
		virtual Bool TryGet();
		/** semaphore will invalid if 
			set i_set < 0 or
			i_max < i_set or
			i_max > MAXIMUM_COUNT  */
		Semaphore(Int32 i_set, Int32 i_max);
		~Semaphore();
		Bool SetMax(Int32 i_max);
		Int32 Current();
		Int32 Maximum();
		Bool Wait(Time::TimeUnit t);
		/** return false if i_num <= 0 or i_num add to current count
			larger than maximum count */
		Bool Post(Int32 i_num = 1);
	public:
		static const Int32 MAXIMUM_COUNT = 2109876543;
	private:
		AnyPtr _smo;
	};

}
}

#endif
