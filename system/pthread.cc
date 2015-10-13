#include "pthread.hh"
#include "system/synchronization/event.hh"
#include "base/memops.hh"

namespace Prot {
namespace System {

	class PoolThreadData {
	public:
		PoolThreadData() :
		sig_in(False),
		sig_out(True){}
	public:
		/* Lock::Mutex lock; */
		Lock::ManualResetEvent sig_in;
		Lock::ManualResetEvent sig_out;
		THREADSTARTINGENTRY func;
		Void* args;
		Int32 flag;
	};

#define PTDFLAG_STARTING 1
#define PTDFLAG_HAS_EXIT 2
#define PTDFLAG_DETACHED 4
#define PTDFLAG_ACTIVING 8

	Void _Prot_WrapPoolThread_Proc_(Void* args)
	{
		PoolThreadData* ptd = static_cast<PoolThreadData*>(args);
		while (!(ptd->flag & PTDFLAG_DETACHED))
		{
			ptd->sig_in.Wait(Time::TimeUnit::Infinite());
			THREADSTARTINGENTRY th_func = ptd->func;
			if (!th_func)
				break;
			th_func(ptd->args);
			ptd->sig_in.Reset();
			ptd->flag &= ~PTDFLAG_ACTIVING;
			ptd->sig_out.Set();
		}
		if (!(ptd->flag & PTDFLAG_HAS_EXIT))
			delete ptd;
	}

	PoolThread::PoolThread(THREADSTARTINGENTRY func)
	: Thread(_Prot_WrapPoolThread_Proc_)
	{
		PoolThreadData* p = new PoolThreadData();
		p->func = func;
		p->flag = 0;
		_ptr = p;
	}

	PoolThread::~PoolThread()
	{
		PoolThreadData* ptd = static_cast<PoolThreadData*>(_ptr);
		Bool should_free = False;
		if (Thread::Status() != Thread::Offline)
		{
			ptd->flag |= PTDFLAG_DETACHED;
			ptd->func = Null;
			ptd->sig_in.Set();
		}
		else
		{
			delete ptd;
		}
	}

	THREADID PoolThread::Run(Void* args)
	{
		PoolThreadData* ptd = static_cast<PoolThreadData*>(_ptr);
		if (!(ptd->flag & PTDFLAG_STARTING))
		{
			ptd->flag = PTDFLAG_STARTING;
			if (!Thread::Run(_ptr))
				return 0;
		}
		else if (ptd->flag & PTDFLAG_ACTIVING)
		{
			return 0;
		}
		ptd->sig_out.Reset();
		ptd->args = args;
		ptd->flag |= PTDFLAG_ACTIVING;
		ptd->sig_in.Set();
		return GetID();
	}

	Bool PoolThread::Kill()
	{
		Bool stat = Thread::Kill();
		PoolThreadData* ptd = static_cast<PoolThreadData*>(_ptr);
		if (stat)
		{
			ptd->flag = PTDFLAG_HAS_EXIT;
			ptd->sig_out.Set();
		}
		return stat;
	}

	PoolThread::ThreadState PoolThread::Status()
	{
		if (Thread::Status() == Thread::Offline)
			return Offline;
		PoolThreadData* ptd = static_cast<PoolThreadData*>(_ptr);
		return (ptd->flag & PTDFLAG_ACTIVING) ? Running : Holding;
	}

	Bool PoolThread::Wait(Time::TimeUnit t)
	{
		PoolThreadData* ptd = static_cast<PoolThreadData*>(_ptr);
		return ptd->sig_out.Wait(t);
	}

}
}
