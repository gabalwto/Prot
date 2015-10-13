#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

#include "base/allocate.hh"
#include "base/memops.hh"

#include <stdio.h>

namespace Prot {
namespace System {

typedef struct {
	THREADSTARTINGENTRY func;
	void* args;
	THREADID tid;
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t exited;
	Int32 flag;
} THREAD_DATA;
#define PTD(x) ((THREAD_DATA*)x)

#define TDFLAG_STARTING 1
#define TDFLAG_HAS_EXIT 2
#define TDFLAG_DETACHED 4
#define TDFLAG_TICKLOCK 0x40

static pthread_mutex_t tid_mutex = PTHREAD_MUTEX_INITIALIZER;
static UInt32 tid_max = 1;
static pthread_key_t tlskey;
static pthread_once_t tlskey_once = PTHREAD_ONCE_INIT;

inline THREADID _TidRollGenNext()
{
	pthread_mutex_lock(&tid_mutex);
	THREADID tid = (THREADID)tid_max;
	tid_max += 3;
	pthread_mutex_unlock(&tid_mutex);
	return tid;
}

inline void _FreeThreadData(THREAD_DATA* pthd)
{
	pthread_cond_destroy(&pthd->exited);
	pthread_mutex_destroy(&pthd->mutex);
	Memory::Free(pthd);
}

static void tls_keydest_callback_func(void* pthd)
{
	bool should_free = false;
	if (!pthd) return;
	pthread_mutex_lock(&PTD(pthd)->mutex);
	PTD(pthd)->flag |= TDFLAG_HAS_EXIT;
	if (PTD(pthd)->flag & TDFLAG_DETACHED)
		should_free = true;
	pthread_cond_broadcast(&PTD(pthd)->exited);
	pthread_mutex_unlock(&PTD(pthd)->mutex);
	if (should_free)
	{
		_FreeThreadData(PTD(pthd));
	}
}

static void tls_make_key_proc()
{
	pthread_key_create(&tlskey, tls_keydest_callback_func);
}

inline Thread::ThreadState _GetThreadState(pthread_t thread)
{
	int nstat = pthread_kill(thread, 0);
	if (nstat == ESRCH)
	{
		return Thread::Offline;
	}
	else if (nstat == EINVAL)
	{
		// LOG Error Info
		return Thread::Offline;
	}
	return Thread::Running;
}

Thread::Thread(THREADSTARTINGENTRY func)
{
	if (!func)
		func((void*)"thread func is null");
	_thd = Memory::Alloc(sizeof(THREAD_DATA));
	Memory::Fill(_thd, sizeof(THREAD_DATA), 0);
	PTD(_thd)->func = func;
	pthread_mutex_init(&PTD(_thd)->mutex, NULL);
	pthread_condattr_t pct;
	pthread_condattr_init(&pct);
	int ret = pthread_condattr_setclock(&pct, CLOCK_MONOTONIC);
	if (ret == 0)
		PTD(_thd)->flag |= TDFLAG_TICKLOCK;
	pthread_cond_init(&PTD(_thd)->exited,
		             (PTD(_thd)->flag & TDFLAG_TICKLOCK) ? &pct : NULL);
	pthread_condattr_destroy(&pct);
	pthread_once(&tlskey_once, tls_make_key_proc);
}

Thread::~Thread()
{
	bool should_free = false;
	pthread_mutex_lock(&PTD(_thd)->mutex);
	if (_thd)
	{
		PTD(_thd)->flag |= TDFLAG_DETACHED;
		if (PTD(_thd)->flag & TDFLAG_HAS_EXIT)
			should_free = true;
	}
	pthread_mutex_unlock(&PTD(_thd)->mutex);
	if (should_free)
	{
		_FreeThreadData(PTD(_thd));
	}
}

THREADID Thread::GetID()
{
	return PTD(_thd)->tid;
}

static void* _Prot_WrapThreadProc_Posix_(void* pargs)
{
	int cot;
	pthread_setspecific(tlskey, pargs);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &cot);
	THREAD_DATA* ptd = (THREAD_DATA*)pargs;
	THREADSTARTINGENTRY func = ptd->func;
	void* args = ptd->args;
//	ptd->flag |= TDFLAG_STARTING;
	func(args);
	return 0;
}
THREADID Thread::Run(void* args)
{
	if (PTD(_thd)->func == 0)
		return 0;
	if (PTD(_thd)->tid)
	{
		if (!(PTD(_thd)->flag & TDFLAG_HAS_EXIT))
			return 0;
//		if (_GetThreadState(PTD(_thd)->thread))
//			return 0;
		PTD(_thd)->tid = 0;
		PTD(_thd)->thread = 0;
		PTD(_thd)->flag &= TDFLAG_TICKLOCK;
	}

	PTD(_thd)->args = args;
	THREADID tid = _TidRollGenNext();
	pthread_attr_t wrap_thread_attr;
	pthread_attr_init(&wrap_thread_attr);
	pthread_attr_setdetachstate(&wrap_thread_attr, PTHREAD_CREATE_DETACHED);
#if __PT_DEBUG_
	int ret = pthread_create(&PTD(_thd)->thread, &wrap_thread_attr, _Prot_WrapThreadProc_Posix_, _thd);
	if (ret == 0) {
		PTD(_thd)->tid = tid;
	} else {
	 	printf("--------------> thread create failed !! errno: %d\n", ret);
	}
#else
	if (pthread_create(&PTD(_thd)->thread, &wrap_thread_attr, _Prot_WrapThreadProc_Posix_, _thd) == 0) {
		PTD(_thd)->tid = tid;
	}
#endif
//	pthread_attr_destroy(&wrap_thread_attr);
	return PTD(_thd)->tid;
}

Bool Thread::Wait(Time::TimeUnit t)
{
	Bool result = True;
	pthread_mutex_lock(&PTD(_thd)->mutex);
	if (PTD(_thd)->thread && !(PTD(_thd)->flag & TDFLAG_HAS_EXIT))
	{
		int ret;
		if (t.Infinitely())
			ret = pthread_cond_wait(&PTD(_thd)->exited, &PTD(_thd)->mutex);
		else if (!t.Empty()) {
			if (PTD(_thd)->flag & TDFLAG_TICKLOCK) {
				struct timespec ts;
				if (0 == clock_gettime(CLOCK_MONOTONIC, &ts))
				{
					ts.tv_nsec += t.InNanoseconds() % 1000000000;
					ts.tv_sec += t.InSeconds();
					if (ts.tv_nsec >= 1000000000) {
						ts.tv_nsec -= 1000000000;
						ts.tv_sec ++;
					}
					ret = pthread_cond_timedwait(&PTD(_thd)->exited, &PTD(_thd)->mutex, &ts);
				}
				else ret = 0;
			} else {
				struct timespec ts;
				struct timeval tv;
				if (0 == gettimeofday(&tv, NULL))
				{
					ts.tv_sec = tv.tv_sec + t.InSeconds();
					ts.tv_nsec = tv.tv_usec * 1000 + t.InNanoseconds() % 1000000000;
					if (ts.tv_nsec >= 1000000000) {
						ts.tv_nsec -= 1000000000;
						ts.tv_sec ++;
					}
					ret = pthread_cond_timedwait(&PTD(_thd)->exited, &PTD(_thd)->mutex, &ts);
				}
				else ret = 0;
			}
		} else {
			ret = ~ETIMEDOUT;
		}
		if (ret == ETIMEDOUT)
			result = False;
		else if (ret != 0)
		{
			result = _GetThreadState(PTD(_thd)->thread) == Offline;
		}
	}
	pthread_mutex_unlock(&PTD(_thd)->mutex);
	return result;
}

Bool Thread::Kill()
{
	Bool result = False;
	pthread_mutex_lock(&PTD(_thd)->mutex);
	if (PTD(_thd)->thread && !(PTD(_thd)->flag & TDFLAG_HAS_EXIT))
	{
		pthread_cancel(PTD(_thd)->thread);
		pthread_cond_wait(&PTD(_thd)->exited, &PTD(_thd)->mutex);
		result = True;
	}
	pthread_mutex_unlock(&PTD(_thd)->mutex);
	return result;
}

Thread::ThreadState Thread::Status()
{
	// if (PTD(_thd)->thread)
	// 	return _GetThreadState(PTD(_thd)->thread);
	// return Thread::Offline;
	if (PTD(_thd)->thread && !(PTD(_thd)->flag & TDFLAG_HAS_EXIT))
		return Thread::Running;
	return Thread::Offline;
}

/*** This Thread ***************************************/
	
THREADID ThisThread::GetID()
{
	void* pargs = pthread_getspecific(tlskey);
	if (pargs) {
		return PTD(pargs)->tid;
	}
	return (THREADID)pthread_self();
}

Void ThisThread::Sleep(Time::TimeUnit t)
{
	if (t.Empty())
		sched_yield();
	else {
		ULong i_sec = t.InSeconds();
		UInt32 u_sec = ((UInt32)t.InMacroseconds()) % 1000000;
		if (i_sec) sleep(i_sec);
		if (u_sec) usleep(u_sec);
	}
}

Void ThisThread::Sleep(Int32 msec)
{
	if (msec == 0)
		sched_yield();
	else {
		Int32 i_sec = msec / 1000;
		Int32 u_sec = (msec % 1000) * 1000;
		if (i_sec) sleep(i_sec);
		if (u_sec) usleep(u_sec);
	}
}

}
}
