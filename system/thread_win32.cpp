#include "base/allocate.hh"
#include "base/memops.hh"

#include <windows.h>

namespace Prot {
namespace System {

	typedef struct {
		THREADSTARTINGENTRY func;
		THREADID tid;
		HANDLE thread;
		Void* args;
		Int32 flag;
	} THREAD_DATA;
	#define PTD(x) ((THREAD_DATA*)x)

#define TDFLAG_STARTING 1
#define TDFLAG_HAS_EXIT 2

	inline Thread::ThreadState _GetThreadState(HANDLE hThread)
	{
		DWORD nStatus;
		if (GetExitCodeThread(hThread, &nStatus) && nStatus == STILL_ACTIVE)
			return Thread::Running;
		return Thread::Offline;
	}

	Thread::Thread(THREADSTARTINGENTRY func)
	{
		if (!func)
			func("thread func is null");
		_thd = Memory::Alloc(sizeof(THREAD_DATA));
		Memory::Fill(_thd, sizeof(THREAD_DATA), 0);
		PTD(_thd)->func = func;
	}

	Thread::~Thread()
	{
		if (PTD(_thd)->thread)
		{
			while (!(PTD(_thd)->flag & TDFLAG_STARTING))
				Sleep(10);
			CloseHandle(PTD(_thd)->thread);
		}
		Memory::Free(_thd);
	}

	THREADID Thread::GetID()
	{
		return PTD(_thd)->tid;
	}

	DWORD WINAPI _Prot_WrapThreadProc_Win32_(LPVOID lpParameter)
	{
		THREAD_DATA* ptd = (THREAD_DATA*)lpParameter;
		THREADSTARTINGENTRY func = ptd->func;
		void* args = ptd->args;
		ptd->flag |= TDFLAG_STARTING;
		func(args);
		return 0;
	}

	THREADID Thread::Run(void* args)
	{
		if (PTD(_thd)->func == 0)
			return 0;
		if (PTD(_thd)->tid)
		{
			if (_GetThreadState(PTD(_thd)->thread))
				return 0;
			PTD(_thd)->tid = 0;
			CloseHandle(PTD(_thd)->thread);
			PTD(_thd)->thread = 0;
			PTD(_thd)->flag = 0;
		}
		DWORD nTid;
		PTD(_thd)->args = args;
		PTD(_thd)->thread = CreateThread(NULL, 128*1024, _Prot_WrapThreadProc_Win32_, _thd, SYNCHRONIZE|STACK_SIZE_PARAM_IS_A_RESERVATION, &nTid);
		if (PTD(_thd)->thread)
		{
			PTD(_thd)->tid = (THREADID)nTid;
		}
		return PTD(_thd)->tid;
	}

	Bool Thread::Wait(Time::TimeUnit t)
	{
		if (PTD(_thd)->thread)
		{
			DWORD nStatus = WaitForSingleObject(PTD(_thd)->thread, t.InMilliseconds());
			if (nStatus == WAIT_OBJECT_0)
				return true;
			if (nStatus == WAIT_TIMEOUT)
				return false;
			return (_GetThreadState(PTD(_thd)->thread) == Offline);
		}
		return true;
	}

	Bool Thread::Kill()
	{
		if (PTD(_thd)->thread)
		{
			if (TerminateThread(PTD(_thd)->thread, 0))
			{
				PTD(_thd)->tid = 0;
				CloseHandle(PTD(_thd)->thread);
				PTD(_thd)->thread = 0;
				PTD(_thd)->flag = 0;
				return true;
			}
		}
		return false;
	}

	Thread::ThreadState Thread::Status()
	{
		if (PTD(_thd)->thread)
			return _GetThreadState(PTD(_thd)->thread);
		return Offline;
	}

/*** This Thread ***************************************/
	
	THREADID ThisThread::GetID()
	{
		return (THREADID)GetCurrentThreadId();
	}

	Void ThisThread::Sleep(Time::TimeUnit t)
	{
		Int64 tsec = t.InSeconds();
		while (tsec > 86400) //24 * 3600
		{
			::Sleep(86400000);
			tsec -= 86400;
		}
		DWORD msec = ((DWORD)tsec) * 1000 + (((DWORD)t.InMilliseconds()) % 1000);
		::Sleep(msec);
	}
	
	Void ThisThread::Sleep(Int32 msec)
	{
		::Sleep(msec);
	}

}
}
