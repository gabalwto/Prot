#include <test/ptutest.h>
#include "event.hh"
#include <system/thread.hh>
#include <system/time/tickcount.hh>

/** test init */
PTest(Event, TestConstructre)
{
	{
		Lock::Event<Lock::Auto> alock(False);
		EXPECT_EQ(Lock::TypeAutoResetEvent, alock.Type());
	}
	{
		Lock::Event<Lock::Auto> alock(True);
		EXPECT_EQ(Lock::TypeAutoResetEvent, alock.Type());
	}
	{
		Lock::Event<Lock::Manual> alock(False);
		EXPECT_EQ(Lock::TypeManualResetEvent, alock.Type());
	}
	{
		Lock::Event<Lock::Manual> alock(True);
		EXPECT_EQ(Lock::TypeManualResetEvent, alock.Type());
	}
	{
		Lock::AutoResetEvent alock(False);
		EXPECT_EQ(Lock::TypeAutoResetEvent, alock.Type());
	}
	{
		Lock::AutoResetEvent alock(True);
		EXPECT_EQ(Lock::TypeAutoResetEvent, alock.Type());
	}
	{
		Lock::ManualResetEvent alock(False);
		EXPECT_EQ(Lock::TypeManualResetEvent, alock.Type());
	}
	{
		Lock::ManualResetEvent alock(True);
		EXPECT_EQ(Lock::TypeManualResetEvent, alock.Type());
	}
}

typedef struct {
	Lock::ILock* plock;
	Lock::IEvent* pevt;
	Int32 EnterCount;
} MyTestReturnValueData;

Void MyTestSignalResetFunc(Void* args)
{
	MyTestReturnValueData* pdata = (MyTestReturnValueData*)args;
	pdata->pevt->Wait(Time::TimeUnit::Infinite());
	pdata->plock->Get();
	pdata->EnterCount++;
	pdata->plock->Free();
}

#define TSRINTVAL 100
PTest(AutoResetEvent, TestSignalReset)
{
	{
		Lock::Mutex lock;
		MyTestReturnValueData mydata;
		Lock::AutoResetEvent aevt(True);
		mydata.pevt = &aevt;
		mydata.plock = &lock;
		mydata.EnterCount = 0;
		for (int i = 0; i < 4; i++)
		{
			System::Thread test_thread(MyTestSignalResetFunc);
			System::THREADID tid = test_thread.Run(&mydata);
			EXPECT_NE(0, tid); if (!tid) return;
		}
		System::ThisThread::Sleep(TSRINTVAL);
		EXPECT_EQ(1, mydata.EnterCount);
		aevt.Set();
		System::ThisThread::Sleep(TSRINTVAL);
		EXPECT_EQ(2, mydata.EnterCount);
		aevt.Set();
		System::ThisThread::Sleep(TSRINTVAL);
		EXPECT_EQ(3, mydata.EnterCount);
		aevt.Set();
		System::ThisThread::Sleep(TSRINTVAL);
		EXPECT_EQ(4, mydata.EnterCount);
		aevt.Set();
		System::ThisThread::Sleep(TSRINTVAL);
		EXPECT_EQ(4, mydata.EnterCount);
	}
	{
		Lock::Mutex lock;
		MyTestReturnValueData mydata;
		Lock::AutoResetEvent aevt(False);
		mydata.pevt = &aevt;
		mydata.plock = &lock;
		mydata.EnterCount = 0;
		for (int i = 0; i < 4; i++)
		{
			System::Thread test_thread(MyTestSignalResetFunc);
			System::THREADID tid = test_thread.Run(&mydata);
			EXPECT_NE(0, tid); if (!tid) return;
		}
		System::ThisThread::Sleep(TSRINTVAL);
		EXPECT_EQ(0, mydata.EnterCount);
		aevt.Set();
		System::ThisThread::Sleep(TSRINTVAL);
		EXPECT_EQ(1, mydata.EnterCount);
		aevt.Set();
		System::ThisThread::Sleep(TSRINTVAL);
		EXPECT_EQ(2, mydata.EnterCount);
		aevt.Set();
		System::ThisThread::Sleep(TSRINTVAL);
		EXPECT_EQ(3, mydata.EnterCount);
		aevt.Set();
		System::ThisThread::Sleep(TSRINTVAL);
		EXPECT_EQ(4, mydata.EnterCount);
		aevt.Set();
		System::ThisThread::Sleep(TSRINTVAL);
		EXPECT_EQ(4, mydata.EnterCount);
	}
}

PTest(ManualResetEvent, TestSignalReset)
{
	{
		Lock::Mutex lock;
		MyTestReturnValueData mydata;
		Lock::ManualResetEvent aevt(True);
		mydata.pevt = &aevt;
		mydata.plock = &lock;
		mydata.EnterCount = 0;
		for (int i = 0; i < 4; i++)
		{
			System::Thread test_thread(MyTestSignalResetFunc);
			System::THREADID tid = test_thread.Run(&mydata);
			EXPECT_NE(0, tid); if (!tid) return;
		}
		System::ThisThread::Sleep(150);
		EXPECT_EQ(4, mydata.EnterCount);
		aevt.Set();
		System::ThisThread::Sleep(100);
		EXPECT_EQ(4, mydata.EnterCount);
	}
	{
		Lock::Mutex lock;
		MyTestReturnValueData mydata;
		Lock::ManualResetEvent aevt(False);
		mydata.pevt = &aevt;
		mydata.plock = &lock;
		mydata.EnterCount = 0;
		for (int i = 0; i < 4; i++)
		{
			System::Thread test_thread(MyTestSignalResetFunc);
			System::THREADID tid = test_thread.Run(&mydata);
			EXPECT_NE(0, tid); if (!tid) return;
		}
		System::ThisThread::Sleep(100);
		EXPECT_EQ(0, mydata.EnterCount);
		aevt.Set();
		System::ThisThread::Sleep(100);
		EXPECT_EQ(4, mydata.EnterCount);
		aevt.Set();
		System::ThisThread::Sleep(100);
		EXPECT_EQ(4, mydata.EnterCount);
	}
}
