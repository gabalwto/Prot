#include <test/ptutest.h>
#include "semaphore.hh"
#include <system/thread.hh>
#include <system/time/tickcount.hh>

/** test init */
PTest(Semaphore, TestILockInit)
{
	{
		Lock::Semaphore semlock(0, 0);
		EXPECT_EQ(Lock::TypeInvalid, semlock.Type());
	}
	{
		Lock::Semaphore semlock(0, 1);
		EXPECT_EQ(Lock::TypeSemaphore, semlock.Type());
	}
	{
		Lock::Semaphore semlock(1, 0);
		EXPECT_EQ(Lock::TypeInvalid, semlock.Type());
	}
	{
		Lock::Semaphore semlock(1, 1);
		EXPECT_EQ(Lock::TypeSemaphore, semlock.Type());
	}
	{
		Lock::Semaphore semlock(2, 1);
		EXPECT_EQ(Lock::TypeInvalid, semlock.Type());
	}
	{
		Lock::Semaphore semlock(100, 1);
		EXPECT_EQ(Lock::TypeInvalid, semlock.Type());
	}
	{
		Lock::Semaphore semlock(0, -1);
		EXPECT_EQ(Lock::TypeInvalid, semlock.Type());
	}
	{
		Lock::Semaphore semlock(100, -1);
		EXPECT_EQ(Lock::TypeInvalid, semlock.Type());
	}
	{
		Lock::Semaphore semlock(0, 0x7fffffff);
		EXPECT_EQ(Lock::TypeInvalid, semlock.Type());
	}
	{
		Lock::Semaphore semlock(100, 0x7fffffff);
		EXPECT_EQ(Lock::TypeInvalid, semlock.Type());
	}
	{
		Lock::Semaphore semlock(0, 0x80000000);
		EXPECT_EQ(Lock::TypeInvalid, semlock.Type());
	}
	{
		Lock::Semaphore semlock(100, 0x80000000);
		EXPECT_EQ(Lock::TypeInvalid, semlock.Type());
	}
	{
		Lock::Semaphore semlock(0, Lock::Semaphore::MAXIMUM_COUNT-1);
		EXPECT_EQ(Lock::TypeSemaphore, semlock.Type());
	}
	{
		Lock::Semaphore semlock(0, Lock::Semaphore::MAXIMUM_COUNT+1);
		EXPECT_EQ(Lock::TypeInvalid, semlock.Type());
	}
	{
		Lock::Semaphore semlock(0, Lock::Semaphore::MAXIMUM_COUNT);
		EXPECT_EQ(Lock::TypeSemaphore, semlock.Type());
	}
	{
		Lock::Semaphore semlock(50, 100);
		EXPECT_EQ(Lock::TypeSemaphore, semlock.Type());
	}
	{
		Lock::Semaphore semlock(100, 100);
		EXPECT_EQ(Lock::TypeSemaphore, semlock.Type());
	}
	{
		Lock::Semaphore semlock(100, Lock::Semaphore::MAXIMUM_COUNT);
		EXPECT_EQ(Lock::TypeSemaphore, semlock.Type());
	}
}

/** test return value */
typedef struct {
	Lock::Semaphore* psem;
	Bool ReturnValue1;
	Int32 CurrentValue1;
	Bool ReturnValue2;
	Int32 CurrentValue2;
	Bool ReturnValue3;
	Int32 CurrentValue3;
	Bool ReturnValue4;
	Int32 CurrentValue4;
	Bool ReturnValue5;
	Int32 CurrentValue5;
	Bool Finished;
} MyTestReturnValueData;

Void MyTestSemaphoreReturnValueFunc(Void* args)
{
	MyTestReturnValueData* pdata = (MyTestReturnValueData*)args;
	Lock::Semaphore& mysem = *pdata->psem;
	pdata->ReturnValue1 = mysem.Wait(Time::TimeDelta::FromMilliseconds(200));
	pdata->CurrentValue1 = mysem.Current();
	pdata->ReturnValue2 = mysem.Wait(Time::TimeDelta::FromMilliseconds(200));
	pdata->CurrentValue2 = mysem.Current();
	pdata->ReturnValue3 = mysem.Wait(Time::TimeDelta::FromMilliseconds(0));
	pdata->CurrentValue3 = mysem.Current();
	pdata->ReturnValue4 = mysem.Wait(Time::TimeDelta::FromMilliseconds(200));
	pdata->CurrentValue4 = mysem.Current();
	pdata->ReturnValue5 = mysem.Wait(Time::TimeDelta::FromMilliseconds(300));
	pdata->CurrentValue5 = mysem.Current();
	pdata->Finished = True;
}

PTest(Semaphore, TestReturnValue)
{
	Lock::Semaphore semlock(3, 4);
	EXPECT_EQ(Lock::TypeSemaphore, semlock.Type());
	EXPECT_EQ(3, semlock.Current());
	EXPECT_EQ(4, semlock.Maximum());
	EXPECT_TRUE(semlock.SetMax(1));
	EXPECT_EQ(1, semlock.Maximum());
	EXPECT_TRUE(semlock.SetMax(10));
	EXPECT_EQ(10, semlock.Maximum());
	EXPECT_FALSE(semlock.SetMax(0));
	EXPECT_EQ(10, semlock.Maximum());
	EXPECT_FALSE(semlock.SetMax(-1));
	EXPECT_EQ(10, semlock.Maximum());
	EXPECT_FALSE(semlock.SetMax(0x7fffffff));
	EXPECT_EQ(10, semlock.Maximum());
	EXPECT_FALSE(semlock.SetMax(Lock::Semaphore::MAXIMUM_COUNT+1));
	EXPECT_EQ(10, semlock.Maximum());
	EXPECT_TRUE(semlock.SetMax(Lock::Semaphore::MAXIMUM_COUNT));
	EXPECT_EQ(Lock::Semaphore::MAXIMUM_COUNT, semlock.Maximum());
	EXPECT_TRUE(semlock.SetMax(Lock::Semaphore::MAXIMUM_COUNT-1));
	EXPECT_EQ(Lock::Semaphore::MAXIMUM_COUNT-1, semlock.Maximum());
	EXPECT_TRUE(semlock.SetMax(3));
	EXPECT_EQ(3, semlock.Maximum());

	MyTestReturnValueData mydata;
	mydata.psem = &semlock;
	mydata.ReturnValue1 = False;
	mydata.ReturnValue2 = False;
	mydata.ReturnValue3 = False;
	mydata.ReturnValue4 = False;
	mydata.CurrentValue1 = -1;
	mydata.CurrentValue2 = -1;
	mydata.CurrentValue3 = -1;
	mydata.CurrentValue4 = -1;
	mydata.Finished = False;
	System::Thread test_thread(MyTestSemaphoreReturnValueFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(50)));
	EXPECT_TRUE(mydata.ReturnValue1);
	EXPECT_EQ(2, mydata.CurrentValue1);
	EXPECT_TRUE(mydata.ReturnValue2);
	EXPECT_EQ(1, mydata.CurrentValue2);
	EXPECT_TRUE(mydata.ReturnValue3);
	EXPECT_EQ(0, mydata.CurrentValue3);
	EXPECT_FALSE(mydata.ReturnValue4);
	EXPECT_EQ(-1, mydata.CurrentValue4);
	EXPECT_FALSE(mydata.Finished);
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(300)));
	EXPECT_FALSE(mydata.ReturnValue4);
	EXPECT_EQ(0, mydata.CurrentValue4);
	EXPECT_FALSE(mydata.Finished);
	semlock.Post(2);
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(400)));
	EXPECT_TRUE(mydata.ReturnValue5);
	EXPECT_EQ(1, mydata.CurrentValue5);
	EXPECT_TRUE(mydata.Finished);
}


typedef struct {
	Lock::Semaphore* psem;
	Lock::ILock* pfc_lock;
	Lock::ILock* pec_lock;
	Int32 EntersCount;
	Int32 FinishCount;
} MyTestSemaphoreMultipleWaitData;

Void MyTestSemaphoreMultipleWaitFunc(Void* args)
{
	MyTestSemaphoreMultipleWaitData* pdata = (MyTestSemaphoreMultipleWaitData*)args;
	pdata->psem->Wait(Time::TimeDelta::Infinite());
	pdata->pec_lock->Get();
	Int32 tidx = pdata->EntersCount++;
	pdata->pec_lock->Free();
	System::ThisThread::Sleep(400);
	pdata->pfc_lock->Get();
	pdata->FinishCount++;
	pdata->pfc_lock->Free();
	pdata->psem->Post(1);
}

PTest(Semaphore, TestMultipleWait)
{
	Lock::Semaphore semlock(0, 4);
	Lock::SinglenterLock lock1;
	Lock::SinglenterLock lock2;
	MyTestSemaphoreMultipleWaitData mydata;
	mydata.pec_lock = &lock1;
	mydata.pfc_lock = &lock2;
	mydata.psem = &semlock;
	mydata.EntersCount = 0;
	mydata.FinishCount = 0;
	for (int i = 0; i < 10; i++)
	{
		System::Thread test_thread(MyTestSemaphoreMultipleWaitFunc);
		System::THREADID tid = test_thread.Run(&mydata);
		EXPECT_NE(0, tid); if (!tid) return;
	}
	System::ThisThread::Sleep(100);
	EXPECT_EQ(0, mydata.EntersCount);
	EXPECT_EQ(0, mydata.FinishCount);
	EXPECT_FALSE(semlock.Post(0));
	System::ThisThread::Sleep(100);
	EXPECT_EQ(0, mydata.EntersCount);
	EXPECT_EQ(0, mydata.FinishCount);
	EXPECT_FALSE(semlock.Post(5));
	System::ThisThread::Sleep(100);
	EXPECT_EQ(0, mydata.EntersCount);
	EXPECT_EQ(0, mydata.FinishCount);
	EXPECT_FALSE(semlock.Post(-1));
	System::ThisThread::Sleep(100);
	EXPECT_EQ(0, mydata.EntersCount);
	EXPECT_EQ(0, mydata.FinishCount);
	EXPECT_TRUE(semlock.Post(4));		// Now starting enter 4 ...
	System::ThisThread::Sleep(180);
	EXPECT_EQ(4, mydata.EntersCount);
	EXPECT_EQ(0, mydata.FinishCount);
	System::ThisThread::Sleep(400);		// Now first 4 enter finished, second 4 has enter
	EXPECT_EQ(8, mydata.EntersCount);
	EXPECT_EQ(4, mydata.FinishCount);
	semlock.SetMax(8);
	EXPECT_TRUE(semlock.Post(3));		// force the last 2 enter semaphore lock
	System::ThisThread::Sleep(120);		// now 6 are in sleeping, 4(300),2(0~120)
	EXPECT_EQ(1, semlock.Current());
	EXPECT_EQ(10, mydata.EntersCount);
	EXPECT_EQ(4, mydata.FinishCount);
	System::ThisThread::Sleep(210);		// now 4(510) has finish, 2(210~330)
	EXPECT_EQ(10, mydata.EntersCount);
	EXPECT_EQ(8, mydata.FinishCount);
	System::ThisThread::Sleep(240);		// now all finished
	EXPECT_EQ(10, mydata.EntersCount);
	EXPECT_EQ(10, mydata.FinishCount);
	EXPECT_EQ(7, semlock.Current());
	EXPECT_EQ(8, semlock.Maximum());
}
