#include "semaphore.hh"
#include <allocate.hh>

#if __PLATFORM_OSYS_ == PTOS_WINDOWS
#include <windows.h>
#else
#include <semaphore.h>
#include <pthread.h>
#include <sys/time.h>
#endif

namespace Prot {
namespace Lock {

#define PSD(x) ((SEMAPHORE_DATA*)x)

#define SEMAPHORE_MAXIMUM_COUNT 2109876543 /* = Semaphore::MAXIMUM_COUNT */

#if __PLATFORM_OSYS_ == PTOS_WINDOWS

	typedef struct {
		HANDLE h_sem;
		LONG current;
		LONG maximum;
	} SEMAPHORE_DATA;

	Semaphore::Semaphore(Int32 i_set, Int32 i_max)
	{
		if (i_set <= i_max && i_set >= 0 && i_max > 0 && i_max <= SEMAPHORE_MAXIMUM_COUNT)
		{
			HANDLE hsem = CreateSemaphore(NULL, i_set, SEMAPHORE_MAXIMUM_COUNT, NULL);
			if (hsem)
			{
				_smo = Memory::Alloc(sizeof(SEMAPHORE_DATA));
				PSD(_smo)->h_sem = hsem;
				PSD(_smo)->current = i_set;
				PSD(_smo)->maximum = i_max;
				return;
			}
		}
		_smo = NullPtr;
	}

	Semaphore::~Semaphore()
	{
		if (_smo)
		{
			Post(PSD(_smo)->maximum - PSD(_smo)->current);
			CloseHandle(PSD(_smo)->h_sem);
			Memory::Free(_smo);
		}
	}

	Bool Semaphore::Wait(Time::TimeUnit t)
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(PSD(_smo)->h_sem, t.InMilliseconds()))
		{
			InterlockedDecrement(&PSD(_smo)->current);
			return True;
		}
		return False;
	}

	Bool Semaphore::Post(Int32 i_num)
	{
		if (i_num <= 0 || PSD(_smo)->current + i_num > PSD(_smo)->maximum)
			return False;
		LONG tmp = InterlockedAdd(&PSD(_smo)->current, i_num);
		if (tmp <= PSD(_smo)->maximum && ReleaseSemaphore(PSD(_smo)->h_sem, i_num, &tmp))
			return True;
		InterlockedAdd(&PSD(_smo)->current, -i_num);
		return False;
	}

#else

	typedef struct {
		pthread_cond_t l_cond;
		pthread_mutex_t l_mtx;
		Int32 current;
		Int32 maximum;
		Int32 flag;
	} SEMAPHORE_DATA;

#define SDFLAG_TICKCLOCK 1

	Semaphore::Semaphore(Int32 i_set, Int32 i_max)
	{
		if (i_set <= i_max && i_set >= 0 && i_max > 0 && i_max <= SEMAPHORE_MAXIMUM_COUNT)
		{
			_smo = Memory::Alloc(sizeof(SEMAPHORE_DATA));
			if (_smo)
			{
				pthread_condattr_t pct;
				pthread_condattr_init(&pct);
				int ret = pthread_condattr_setclock(&pct, CLOCK_MONOTONIC);
				if (0 == ret)
					PSD(_smo)->flag |= SDFLAG_TICKCLOCK;
				ret = pthread_cond_init(&PSD(_smo)->l_cond,
					(PSD(_smo)->flag & SDFLAG_TICKCLOCK) ? &pct : NULL);
				if (0 == ret)
				{
					if (0 == pthread_mutex_init(&PSD(_smo)->l_mtx, NULL))
					{
						PSD(_smo)->current = i_set;
						PSD(_smo)->maximum = i_max;
						return;
					}
					pthread_cond_destroy(&PSD(_smo)->l_cond);
				}
				Memory::Free(_smo);
			}
		}
		_smo = NullPtr;
	}

	Semaphore::~Semaphore()
	{
		if (_smo)
		{
			// Post(PSD(_smo)->maximum - PSD(_smo)->current);
			pthread_cond_destroy(&PSD(_smo)->l_cond);
			pthread_mutex_destroy(&PSD(_smo)->l_mtx);
			Memory::Free(_smo);
		}
	}

    inline Bool _TryWaitSemaphore(SEMAPHORE_DATA* p_d)
	{
		Bool got;
		pthread_mutex_lock(&p_d->l_mtx);
		if (p_d->current) {
			p_d->current --;
			got = True;
		} else {
			got = False;
		}
		pthread_mutex_unlock(&p_d->l_mtx);
		return got;
	}

#include <unistd.h>
	Bool Semaphore::Wait(Time::TimeUnit t)
	{
		int ret, rc;
		struct timeval tv;
		struct timespec ts;
		if (PSD(_smo)->flag & SDFLAG_TICKCLOCK)
			rc = clock_gettime(CLOCK_MONOTONIC, &ts);
		else
			rc = gettimeofday(&tv, NULL);
		// switch
		if (t.Empty())
			return _TryWaitSemaphore(PSD(_smo));
		if (t.Infinitely()) {
			ret = 0;
			pthread_mutex_lock(&PSD(_smo)->l_mtx);
			while (ret == 0)
			{
				if (PSD(_smo)->current) {
					PSD(_smo)->current --;
					break;
				}
				ret = pthread_cond_wait(&PSD(_smo)->l_cond, &PSD(_smo)->l_mtx);
			}
			pthread_mutex_unlock(&PSD(_smo)->l_mtx);
			return ret == 0;
		}
		if (rc != 0) {
			Int64 tsec = t.InSeconds();
			Bool tgot = False;
			while (tsec > 0)
			{
				tgot = _TryWaitSemaphore(PSD(_smo));
				if (tgot)
					break;
				sleep(1);
				tsec--;
			}
			return tgot;
		}

		if (PSD(_smo)->flag & SDFLAG_TICKCLOCK) {
			ts.tv_nsec += t.InNanoseconds() % 1000000000;
			ts.tv_sec += t.InSeconds();
		} else {
			ts.tv_sec = tv.tv_sec + t.InSeconds();
			ts.tv_nsec = tv.tv_usec * 1000 + t.InNanoseconds() % 1000000000;
		}
		if (ts.tv_nsec >= 1000000000) {
			ts.tv_nsec -= 1000000000;
			ts.tv_sec ++;
		}

		ret = 0;
		pthread_mutex_lock(&PSD(_smo)->l_mtx);
		while (ret == 0)
		{
			if (PSD(_smo)->current) {
				PSD(_smo)->current --;
				break;
			}
			ret = pthread_cond_timedwait(&PSD(_smo)->l_cond, &PSD(_smo)->l_mtx, &ts);
		}
		pthread_mutex_unlock(&PSD(_smo)->l_mtx);
		return ret == 0;
	}

	Bool Semaphore::Post(Int32 i_num)
	{
		if (i_num <= 0 || PSD(_smo)->current + i_num > PSD(_smo)->maximum)
			return False;
		int ret;
		pthread_mutex_lock(&PSD(_smo)->l_mtx);
		if (PSD(_smo)->current + i_num <= PSD(_smo)->maximum) {
			PSD(_smo)->current += i_num;
			if (i_num == 1)
				ret = pthread_cond_signal(&PSD(_smo)->l_cond);
			else
				ret = pthread_cond_broadcast(&PSD(_smo)->l_cond);
			if (ret != 0)
				PSD(_smo)->current -= i_num;				
		} else
			ret = -1;
		pthread_mutex_unlock(&PSD(_smo)->l_mtx);
		return ret == 0;
	}

#endif


	Int32 Semaphore::Current()
	{
		return PSD(_smo)->current;
	}

	Int32 Semaphore::Maximum()
	{
		return PSD(_smo)->maximum;
	}

	Bool Semaphore::SetMax(Int32 i_max)
	{
		if (i_max > 0 && i_max <= SEMAPHORE_MAXIMUM_COUNT)
		{
			PSD(_smo)->maximum = i_max;
			return True;
		}
		return False;
	}


	LockType Semaphore::Type()
	{
		return _smo ? Lock::TypeSemaphore : Lock::TypeInvalid;
	}

	Bool Semaphore::Get()
	{
		return Wait(Time::TimeDelta::Infinite());
	}

	Bool Semaphore::TryGet()
	{
		return Wait(Time::TimeDelta::Zero());
	}

	Void Semaphore::Free()
	{
		Post(1);
	}

}
}
