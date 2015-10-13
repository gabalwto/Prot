#include "event.hh"

#if __PLATFORM_OSYS_ == PTOS_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#include <allocate.hh>
#include <system/debug.hh>
#include <sys/time.h>
#include <errno.h>
#endif

namespace Prot {
namespace Lock {


#if __PLATFORM_OSYS_ == PTOS_WINDOWS
	
	IEvent::IEvent(EventType event_type, Bool init_state)
	{
		HANDLE hEvent = CreateEvent(NULL, event_type, init_state, NULL);
		_eso = hEvent;
	}

	IEvent::~IEvent()
	{
		if (_eso) CloseHandle(_eso);
	}

	Void IEvent::Set()
	{
		SetEvent(_eso);
	}

	Void IEvent::Reset()
	{
		ResetEvent(_eso);
	}

	Bool IEvent::Wait(Time::TimeUnit t)
	{
		return WAIT_OBJECT_0 == WaitForSingleObject(_eso, t.InMilliseconds());
	}

#else

#define EDFLAG_MANURESET 1
#define EDFLAG_TICKCLOCK 4

	typedef struct {
		UInt8 flag;
		Bool state;
		pthread_mutex_t mutex;
		pthread_cond_t cond;
	} EVENT_DATA;

#define PEO(x) ((EVENT_DATA*)x)

	IEvent::IEvent(EventType event_type, Bool init_state)
	{
		EVENT_DATA* pt = (EVENT_DATA*)Memory::Alloc(sizeof(EVENT_DATA));
		do {
			if (pt == NULL)
				break;
			pt->state = init_state;
			if (event_type == Manual)
				pt->flag = EDFLAG_MANURESET;
			else
				pt->flag = 0;
			if (pthread_mutex_init(&pt->mutex, NULL))
				break;
			pthread_condattr_t pct;
			pthread_condattr_init(&pct);
			int ret = pthread_condattr_setclock(&pct, CLOCK_MONOTONIC);
			if (ret == 0)
				pt->flag |= EDFLAG_TICKCLOCK;
			ret = pthread_cond_init(&pt->cond,
				(pt->flag & EDFLAG_TICKCLOCK) ? &pct : NULL);
			pthread_condattr_destroy(&pct);
			if (ret != 0) {
				pthread_mutex_destroy(&pt->mutex);
				break;
			}
			_eso = pt;
			return;
		} while (0);
		if (pt) Memory::Free(pt);
		_eso = NullPtr;
	}

	IEvent::~IEvent()
	{
		if (_eso)
		{
			pthread_cond_destroy(&PEO(_eso)->cond);
			pthread_mutex_destroy(&PEO(_eso)->mutex);
			Memory::Free(_eso);
		}
	}

	Void IEvent::Set()
	{
		if (pthread_mutex_lock(&PEO(_eso)->mutex) != 0)
			System::Debug::Break();
		PEO(_eso)->state = True;
		if (PEO(_eso)->flag & EDFLAG_MANURESET) {
			if (pthread_cond_broadcast(&PEO(_eso)->cond) != 0)
				System::Debug::Break();
		} else {
			if (pthread_cond_signal(&PEO(_eso)->cond) != 0)
				System::Debug::Break();
		}
		if (pthread_mutex_unlock(&PEO(_eso)->mutex) != 0)
			System::Debug::Break();
	}

	Void IEvent::Reset()
	{
		if (pthread_mutex_lock(&PEO(_eso)->mutex) != 0)
			System::Debug::Break();
		PEO(_eso)->state = False;
		if (pthread_mutex_unlock(&PEO(_eso)->mutex) != 0)
			System::Debug::Break();
	}

	Bool IEvent::Wait(Time::TimeUnit t)
	{
		Bool stat_ok = False;
		if (pthread_mutex_lock(&PEO(_eso)->mutex) != 0)
			System::Debug::Break();
		if (t.Empty())
		{
			stat_ok = PEO(_eso)->state;
		}
		else if (t.Infinitely())
		{
			while (!PEO(_eso)->state)
			{
				if (pthread_cond_wait(&PEO(_eso)->cond, &PEO(_eso)->mutex) != 0)
				{
					pthread_mutex_unlock(&PEO(_eso)->mutex);
					return False;
				}
			}
			stat_ok = True;
		}
		else
		{
			int rc;
			struct timespec ts;
			struct timeval tv;
			if (PEO(_eso)->flag & EDFLAG_TICKCLOCK)
			{
				rc = clock_gettime(CLOCK_MONOTONIC, &ts);
				if (rc == 0)
				{
					
					ts.tv_nsec += t.InNanoseconds() % 1000000000;
					ts.tv_sec += t.InSeconds();
				}
			}
			else
			{
				rc = gettimeofday(&tv, NULL);
				if (rc == 0)
				{
					ts.tv_sec = tv.tv_sec + t.InSeconds();
					ts.tv_nsec = tv.tv_usec * 1000 + t.InNanoseconds() % 1000000000;
				}
			}
			if (rc == 0)
			{
				if (ts.tv_nsec >= 1000000000) {
					ts.tv_nsec -= 1000000000;
					ts.tv_sec ++;
				}
				while (!PEO(_eso)->state)
				{
					rc = pthread_cond_timedwait(&PEO(_eso)->cond, &PEO(_eso)->mutex, &ts);
					if (rc == ETIMEDOUT)
						break;
					if (rc)
					{
						pthread_mutex_unlock(&PEO(_eso)->mutex);
						return False;
					}
				}
				stat_ok = PEO(_eso)->state;
			}
		}
		if (stat_ok && !(PEO(_eso)->flag & EDFLAG_MANURESET))
			PEO(_eso)->state = False;
		if (pthread_mutex_unlock(&PEO(_eso)->mutex) != 0)
			System::Debug::Break();
		return stat_ok;
	}

#endif

	LockType IEvent::Type()
	{
		return Lock::TypeInvalid;
	}

	Bool IEvent::Get()
	{
		return Wait(Time::TimeUnit::Infinite());
	}

	Bool IEvent::TryGet()
	{
		return Wait(Time::TimeUnit::Zero());
	}

	Void IEvent::Free()
	{
		Set();
	}


}
}
