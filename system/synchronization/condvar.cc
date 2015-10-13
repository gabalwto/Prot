#include "condvar.hh"

#include <allocate.hh>
#include <system/synchronization/event.hh>
#include <system/synchronization/semaphore.hh>

namespace Prot {
namespace Lock {

	class CONDITION_VARIABLE_DATA
	{
	public:
		CONDITION_VARIABLE_DATA(ILock* l) :	lock(l),
			semap(0, Semaphore::MAXIMUM_COUNT),
			mrend(False)	{
			wait_count = wait_flags = 0;
		}
		ILock* lock;
		Semaphore semap;
		RecursiveMutex mtxi;
		RecursiveMutex mtxd;
		ManualResetEvent mrend;
		Int32 wait_count;
		Int32 wait_flags;
	};

#define CVFLAG_WAKE_ALL 1

	ConditionVariable::ConditionVariable(ILock* lock)
	{
		_cvo = new CONDITION_VARIABLE_DATA(lock);
	}

	ConditionVariable::~ConditionVariable()
	{
		CONDITION_VARIABLE_DATA* p = static_cast<CONDITION_VARIABLE_DATA*>(_cvo);
		delete p;
	}

	Bool ConditionVariable::Wait(Time::TimeUnit t)
	{
		CONDITION_VARIABLE_DATA* p = static_cast<CONDITION_VARIABLE_DATA*>(_cvo);
		Bool rv, rn;
		p->mtxi.Get();
		p->mtxd.Get();
		p->wait_count++;
		p->mtxd.Free();
		p->mtxi.Free();

		if (p->lock)
			p->lock->Free(); 
		rv = p->semap.Wait(t);
	
		p->mtxd.Get();
		p->wait_count--;
		rn = (p->wait_count == 0) && (p->wait_flags & CVFLAG_WAKE_ALL);
		// use comment code line bellow and in WakeOne function, if need
		//     Wait return after WakeOne called, may no helpful.
		// rn = !(p->wait_flags & CVFLAG_WAKE_ALL) || (p->wait_count == 0);
		p->mtxd.Free();

		if (rn)
			p->mrend.Set();
		if (p->lock)
			p->lock->Get();
		return rv;
	}

	Int32 ConditionVariable::WakeOne()
	{
		CONDITION_VARIABLE_DATA* p = static_cast<CONDITION_VARIABLE_DATA*>(_cvo);
		p->mtxi.Get();
		Int32 n = 0;
		if (p->wait_count)
		{
			p->semap.Post();
			n = 1;
		}
		p->mtxi.Free();
		return n;
		// CONDITION_VARIABLE_DATA* p = static_cast<CONDITION_VARIABLE_DATA*>(_cvo);
		// p->mtxi.Get();
		// p->mtxd.Get();
		// Int32 n = 0;
		// if (p->wait_count) {
		// 	p->semap.Post();
		// 	p->mtxd.Free();
		// 	p->mrend.Wait(Time::TimeUnit::Infinite());
		// 	p->mrend.Reset();
		// 	n = 1;
		// } else
		// 	p->mtxd.Free();
		// p->mtxi.Free();
		// return n;
	}

	Int32 ConditionVariable::WakeAll()
	{
		CONDITION_VARIABLE_DATA* p = static_cast<CONDITION_VARIABLE_DATA*>(_cvo);
		p->mtxi.Get();
		p->mtxd.Get();
		Int32 n = p->wait_count;
		if (n) {
			p->wait_flags |= CVFLAG_WAKE_ALL;
			p->semap.Post(n);
			p->mtxd.Free();
			p->mrend.Wait(Time::TimeUnit::Infinite());
			p->mrend.Reset();
			p->wait_flags &= ~CVFLAG_WAKE_ALL;
		} else
			p->mtxd.Free();
		p->mtxi.Free();
		return n;
	}

	LockType ConditionVariable::Type()
	{
		return TypeConditionVariable;
	}

	Bool ConditionVariable::Get()
	{
		return Wait(Time::TimeUnit::Infinite());
	}

	Void ConditionVariable::Free()
	{
		WakeOne();
	}

	Bool ConditionVariable::TryGet()
	{
		return Wait(Time::TimeUnit::Zero());
	}

}
}
