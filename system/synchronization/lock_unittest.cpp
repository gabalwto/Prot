#include <test/ptutest.h>
#include <system/thread.hh>
#include <system/time/tickcount.hh>

#include "lock.hh"
#include "semaphore.hh"
#include "event.hh"
#include "condvar.hh"

PTest(ILock, TestNonTypeInvalid)
{
	Lock::SinglenterLock lock1;
	EXPECT_NE(Lock::TypeInvalid, lock1.Type());
	Lock::RecursiveLock lock2;
	EXPECT_NE(Lock::TypeInvalid, lock2.Type());
	Lock::SpinLock lock3;
	EXPECT_NE(Lock::TypeInvalid, lock3.Type());
}

/** test return value */
typedef struct {
	Lock::ILock* test_lock;
	Bool GetReturn;
	Bool TryGetReturn;
	Bool Finished;
} MyTestReturnValueData;

Void MyTestReturnValueFunc(Void* args)
{
	MyTestReturnValueData* pdata = (MyTestReturnValueData*)args;
	Lock::ILock* plock = pdata->test_lock;
	pdata->GetReturn = plock->Get();
	pdata->TryGetReturn = plock->TryGet();
	if (pdata->GetReturn)
		plock->Free();
	if (pdata->TryGetReturn)
		plock->Free();
	pdata->Finished = True;
}

PTest(SinglenterLock, TestReturnValue)
{
	Lock::SinglenterLock lock;
	MyTestReturnValueData mydata;
	mydata.test_lock = &lock;
	mydata.GetReturn = False;
	mydata.TryGetReturn = False;
	mydata.Finished = False;
	EXPECT_EQ(Lock::TypeSinglenter, lock.Type());
	System::Thread test_thread(MyTestReturnValueFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_TRUE(mydata.Finished);
	EXPECT_TRUE(mydata.GetReturn);
	EXPECT_FALSE(mydata.TryGetReturn);

	mydata.GetReturn = False;
	mydata.TryGetReturn = False;
	mydata.Finished = False;
	Bool _TryGetReturn1 = lock.TryGet();
	EXPECT_TRUE(_TryGetReturn1);
	Bool _TryGetReturn2 = lock.TryGet();
	EXPECT_FALSE(_TryGetReturn2);
	tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_FALSE(mydata.Finished);
	EXPECT_FALSE(mydata.GetReturn);
	EXPECT_FALSE(mydata.TryGetReturn);
	if (_TryGetReturn1)
		lock.Free();
	if (_TryGetReturn2)
		lock.Free();
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::Infinite()));
	EXPECT_TRUE(mydata.Finished);
	EXPECT_TRUE(mydata.GetReturn);
	EXPECT_FALSE(mydata.TryGetReturn);
}

PTest(RecursiveLock, TestReturnValue)
{
	Lock::RecursiveLock lock;
	MyTestReturnValueData mydata;
	mydata.test_lock = &lock;
	mydata.GetReturn = False;
	mydata.TryGetReturn = False;
	mydata.Finished = False;
	EXPECT_EQ(Lock::TypeRecursive, lock.Type());
	System::Thread test_thread(MyTestReturnValueFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_TRUE(mydata.Finished);
	EXPECT_TRUE(mydata.GetReturn);
	EXPECT_TRUE(mydata.TryGetReturn);

	mydata.GetReturn = False;
	mydata.TryGetReturn = False;
	mydata.Finished = False;
	Bool _TryGetReturn1 = lock.TryGet();
	EXPECT_TRUE(_TryGetReturn1);
	Bool _TryGetReturn2 = lock.TryGet();
	EXPECT_TRUE(_TryGetReturn2);
	tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_FALSE(mydata.Finished);
	EXPECT_FALSE(mydata.GetReturn);
	EXPECT_FALSE(mydata.TryGetReturn);
	if (_TryGetReturn1)
		lock.Free();
	if (_TryGetReturn2)
		lock.Free();
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::Infinite()));
	EXPECT_TRUE(mydata.Finished);
	EXPECT_TRUE(mydata.GetReturn);
	EXPECT_TRUE(mydata.TryGetReturn);
}

PTest(SpinLock, TestReturnValue)
{
	Lock::SpinLock lock;
	MyTestReturnValueData mydata;
	mydata.test_lock = &lock;
	mydata.GetReturn = False;
	mydata.TryGetReturn = False;
	mydata.Finished = False;
	EXPECT_EQ(Lock::TypeSpinning, lock.Type());
	System::Thread test_thread(MyTestReturnValueFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_TRUE(mydata.Finished);
	EXPECT_TRUE(mydata.GetReturn);
	EXPECT_FALSE(mydata.TryGetReturn);

	mydata.GetReturn = False;
	mydata.TryGetReturn = False;
	mydata.Finished = False;
	Bool _TryGetReturn1 = lock.TryGet();
	EXPECT_TRUE(_TryGetReturn1);
	Bool _TryGetReturn2 = lock.TryGet();
	EXPECT_FALSE(_TryGetReturn2);
	tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_FALSE(mydata.Finished);
	EXPECT_FALSE(mydata.GetReturn);
	EXPECT_FALSE(mydata.TryGetReturn);
	if (_TryGetReturn1)
		lock.Free();
	if (_TryGetReturn2)
		lock.Free();
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::Infinite()));
	EXPECT_TRUE(mydata.Finished);
	EXPECT_TRUE(mydata.GetReturn);
	EXPECT_FALSE(mydata.TryGetReturn);
}

PTest(AutoResetEventInitSet, TestReturnValue)
{
	Lock::AutoResetEvent lock(True);
	MyTestReturnValueData mydata;
	mydata.test_lock = &lock;
	mydata.GetReturn = False;
	mydata.TryGetReturn = False;
	mydata.Finished = False;
	EXPECT_EQ(Lock::TypeAutoResetEvent, lock.Type());
	System::Thread test_thread(MyTestReturnValueFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_TRUE(mydata.Finished);
	EXPECT_TRUE(mydata.GetReturn);
	EXPECT_FALSE(mydata.TryGetReturn);

	mydata.GetReturn = False;
	mydata.TryGetReturn = False;
	mydata.Finished = False;
	Bool _TryGetReturn1 = lock.TryGet();
	EXPECT_TRUE(_TryGetReturn1);
	Bool _TryGetReturn2 = lock.TryGet();
	EXPECT_FALSE(_TryGetReturn2);
	tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_FALSE(mydata.Finished);
	EXPECT_FALSE(mydata.GetReturn);
	EXPECT_FALSE(mydata.TryGetReturn);
	if (_TryGetReturn1)
		lock.Free();
	if (_TryGetReturn2)
		lock.Free();
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::Infinite()));
	EXPECT_TRUE(mydata.Finished);
	EXPECT_TRUE(mydata.GetReturn);
	EXPECT_FALSE(mydata.TryGetReturn);
}

PTest(AutoResetEventInitReset, TestReturnValue)
{
	Lock::AutoResetEvent lock(False);
	MyTestReturnValueData mydata;
	mydata.test_lock = &lock;
	mydata.GetReturn = False;
	mydata.TryGetReturn = False;
	mydata.Finished = False;
	EXPECT_EQ(Lock::TypeAutoResetEvent, lock.Type());
	System::Thread test_thread(MyTestReturnValueFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_FALSE(mydata.Finished);
	EXPECT_FALSE(mydata.GetReturn);
	EXPECT_FALSE(mydata.TryGetReturn);
	lock.Set();
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)))
		test_thread.Kill();
	EXPECT_TRUE(mydata.Finished);
	EXPECT_TRUE(mydata.GetReturn);
	EXPECT_FALSE(mydata.TryGetReturn);
}

PTest(ManualResetEventInitSet, TestReturnValue)
{
	Lock::ManualResetEvent lock(True);
	MyTestReturnValueData mydata;
	mydata.test_lock = &lock;
	mydata.GetReturn = False;
	mydata.TryGetReturn = False;
	mydata.Finished = False;
	EXPECT_EQ(Lock::TypeManualResetEvent, lock.Type());
	System::Thread test_thread(MyTestReturnValueFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_TRUE(mydata.Finished);
	EXPECT_TRUE(mydata.GetReturn);
	EXPECT_TRUE(mydata.TryGetReturn);

	mydata.GetReturn = False;
	mydata.TryGetReturn = False;
	mydata.Finished = False;
	Bool _TryGetReturn1 = lock.TryGet();
	EXPECT_TRUE(_TryGetReturn1);
	Bool _TryGetReturn2 = lock.TryGet();
	EXPECT_TRUE(_TryGetReturn2);
	tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_TRUE(mydata.Finished);
	EXPECT_TRUE(mydata.GetReturn);
	EXPECT_TRUE(mydata.TryGetReturn);
	if (_TryGetReturn1)
		lock.Free();
	if (_TryGetReturn2)
		lock.Free();
}

PTest(ManualResetEventInitReset, TestReturnValue)
{
	Lock::ManualResetEvent lock(False);
	MyTestReturnValueData mydata;
	mydata.test_lock = &lock;
	mydata.GetReturn = False;
	mydata.TryGetReturn = False;
	mydata.Finished = False;
	EXPECT_EQ(Lock::TypeManualResetEvent, lock.Type());
	System::Thread test_thread(MyTestReturnValueFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_FALSE(mydata.Finished);
	EXPECT_FALSE(mydata.GetReturn);
	EXPECT_FALSE(mydata.TryGetReturn);
	lock.Set();
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)))
		test_thread.Kill();
	mydata.GetReturn = False;
	mydata.TryGetReturn = False;
	mydata.Finished = False;
	Bool _TryGetReturn1 = lock.TryGet();
	EXPECT_TRUE(_TryGetReturn1);
	Bool _TryGetReturn2 = lock.TryGet();
	EXPECT_TRUE(_TryGetReturn2);
	tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	Bool _Wait_Thread = test_thread.Wait(Time::TimeUnit::FromMilliseconds(250));
	EXPECT_TRUE(_Wait_Thread);
	EXPECT_TRUE(mydata.Finished);
	EXPECT_TRUE(mydata.GetReturn);
	EXPECT_TRUE(mydata.TryGetReturn);
	if (_TryGetReturn1)
		lock.Free();
	if (_TryGetReturn2)
		lock.Free();
	if (!_Wait_Thread)
	{
		lock.Set();
		if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)))
			test_thread.Kill();
	}
}

/** test time waiting */
Void MyTestWaitingFunc(Void* args)
{
	Lock::ILock* plock = (Lock::ILock*)args;
	plock->Get();
	System::ThisThread::Sleep(450);
	plock->Free();
}

PTest(SinglenterLock, TestWaiting)
{
	Lock::SinglenterLock lock;
	System::Thread test_thread(MyTestWaitingFunc);
	UInt64 time_before = Time::TickCount::NowMilliseconds();
	System::THREADID tid = test_thread.Run(&lock);
	EXPECT_NE(0, tid); if (!tid) return;
	System::ThisThread::Sleep(200);
	lock.Get();
	UInt64 time_after = Time::TickCount::NowMilliseconds();
	lock.Free();
	EXPECT_LT(250, time_after - time_before);
	EXPECT_GT(650, time_after - time_before);
}

PTest(RecursiveLock, TestWaiting)
{
	Lock::RecursiveLock lock;
	System::Thread test_thread(MyTestWaitingFunc);
	Long time_before = Time::TickCount::NowMilliseconds();
	System::THREADID tid = test_thread.Run(&lock);
	EXPECT_NE(0, tid); if (!tid) return;
	System::ThisThread::Sleep(200);
	lock.Get();
	Long time_after = Time::TickCount::NowMilliseconds();
	lock.Free();
	EXPECT_LT(250, time_after - time_before);
	EXPECT_GT(650, time_after - time_before);
}

PTest(SpinLock, TestWaiting)
{
	Lock::SpinLock lock;
	System::Thread test_thread(MyTestWaitingFunc);
	Long time_before = Time::TickCount::NowMilliseconds();
	System::THREADID tid = test_thread.Run(&lock);
	EXPECT_NE(0, tid); if (!tid) return;
	System::ThisThread::Sleep(200);
	lock.Get();
	Long time_after = Time::TickCount::NowMilliseconds();
	lock.Free();
	EXPECT_LT(250, time_after - time_before);
	EXPECT_GT(650, time_after - time_before);
}

PTest(Semaphore, TestWaiting11)
{
	Lock::Semaphore lock(1, 1);
	System::Thread test_thread(MyTestWaitingFunc);
	Long time_before = Time::TickCount::NowMilliseconds();
	System::THREADID tid = test_thread.Run(&lock);
	EXPECT_NE(0, tid); if (!tid) return;
	System::ThisThread::Sleep(200);
	lock.Get();
	Long time_after = Time::TickCount::NowMilliseconds();
	lock.Free();
	EXPECT_LT(250, time_after - time_before);
	EXPECT_GT(650, time_after - time_before);
}
PTest(Semaphore, TestWaiting12)
{
	Lock::Semaphore lock(1, 2);
	System::Thread test_thread(MyTestWaitingFunc);
	Long time_before = Time::TickCount::NowMilliseconds();
	System::THREADID tid = test_thread.Run(&lock);
	EXPECT_NE(0, tid); if (!tid) return;
	System::ThisThread::Sleep(200);
	lock.Get();
	Long time_after = Time::TickCount::NowMilliseconds();
	lock.Free();
	EXPECT_LT(250, time_after - time_before);
	EXPECT_GT(650, time_after - time_before);
}
PTest(Semaphore, TestWaiting22)
{
	Lock::Semaphore lock(2, 2);
	System::Thread test_thread(MyTestWaitingFunc);
	Long time_before = Time::TickCount::NowMilliseconds();
	System::THREADID tid = test_thread.Run(&lock);
	EXPECT_NE(0, tid); if (!tid) return;
	System::ThisThread::Sleep(200);
	lock.Get();
	Long time_after = Time::TickCount::NowMilliseconds();
	lock.Free();
	EXPECT_LT(50, time_after - time_before);
	EXPECT_GT(350, time_after - time_before);
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(1000)))
		test_thread.Kill();
}

PTest(AutoResetEvent, TestWaitingT)
{
	Lock::Event<Lock::Auto> lock(True);
	System::Thread test_thread(MyTestWaitingFunc);
	Long time_before = Time::TickCount::NowMilliseconds();
	System::THREADID tid = test_thread.Run(&lock);
	EXPECT_NE(0, tid); if (!tid) return;
	System::ThisThread::Sleep(200);
	lock.Get();
	Long time_after = Time::TickCount::NowMilliseconds();
	lock.Free();
	EXPECT_LT(350, time_after - time_before);
	EXPECT_GT(650, time_after - time_before);
}
PTest(AutoResetEvent, TestWaitingF)
{
	Lock::AutoResetEvent lock(False);
	System::Thread test_thread(MyTestWaitingFunc);
	System::THREADID tid = test_thread.Run(&lock);
	EXPECT_NE(0, tid); if (!tid) return;
	System::ThisThread::Sleep(600);
	EXPECT_FALSE(lock.TryGet());
	Long time_before = Time::TickCount::NowMilliseconds();
	lock.Free();
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(800)));
	Long time_after = Time::TickCount::NowMilliseconds();
	EXPECT_TRUE(lock.TryGet());
	EXPECT_LT(400, time_after - time_before);
	EXPECT_GT(700, time_after - time_before);
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(100)))
		test_thread.Kill();
}

PTest(ManualResetEvent, TestWaitingT)
{
	Lock::Event<Lock::Manual> lock(True);
	System::Thread test_thread(MyTestWaitingFunc);
	Long time_before = Time::TickCount::NowMilliseconds();
	System::THREADID tid = test_thread.Run(&lock);
	EXPECT_NE(0, tid); if (!tid) return;
	System::ThisThread::Sleep(200);
	lock.Get();
	Long time_after = Time::TickCount::NowMilliseconds();
	lock.Free();
	EXPECT_LT(50, time_after - time_before);
	EXPECT_GT(350, time_after - time_before);
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(500)))
		test_thread.Kill();
}
PTest(ManualResetEvent, TestWaitingF)
{
	Lock::ManualResetEvent lock(False);
	System::Thread test_thread(MyTestWaitingFunc);
	System::THREADID tid = test_thread.Run(&lock);
	EXPECT_NE(0, tid); if (!tid) return;
	System::ThisThread::Sleep(600);
	EXPECT_FALSE(lock.TryGet());
	Long time_before = Time::TickCount::NowMilliseconds();
	lock.Free();
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(800)));
	Long time_after = Time::TickCount::NowMilliseconds();
	EXPECT_TRUE(lock.TryGet());
	EXPECT_LT(400, time_after - time_before);
	EXPECT_GT(700, time_after - time_before);
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(100)))
		test_thread.Kill();
}

//PTest(ConditionVariable, TestWaiting)
//{
//	Lock::ConditionVariable lock(Null);
//	System::Thread test_thread(MyTestWaitingFunc);
//	System::THREADID tid = test_thread.Run(&lock);
//	EXPECT_NE(0, tid); if (!tid) return;
//	System::ThisThread::Sleep(100);
//	Long time_before = Time::TickCount::NowMilliseconds();
//	lock.Free();
//	System::ThisThread::Sleep(100);
//	lock.Get();
//	Long time_after = Time::TickCount::NowMilliseconds();
//	lock.Free();
//	EXPECT_LT(250, time_after - time_before);
//	EXPECT_GT(650, time_after - time_before);
//}


/** test calculation */
typedef struct {
	Lock::ILock* calc_lock;
	Lock::ILock* exit_lock;
	long calc_data;
	int end_count;
	bool use_lock;
} MyTestCalculationData;

Void MyTestCalculationFunc(Void* args)
{
	MyTestCalculationData* pdata = (MyTestCalculationData*)args;
	for (int i = 0; i < 3000; i++)
	{
		if (pdata->use_lock) pdata->calc_lock->Get();
		for (int j = 0; j < 100; j++)
		{
			long data = pdata->calc_data;
			data += 1;
			System::ThisThread::Sleep(0);
			pdata->calc_data = data;
		}
		if (pdata->use_lock) pdata->calc_lock->Free();
	}
	pdata->exit_lock->Get();
	pdata->end_count++;
	pdata->exit_lock->Free();
}
#define MyTestCalculationResult 3000000

PTest(SinglenterLock, TestCalculation)
{
	MyTestCalculationData mydata;
	Lock::SinglenterLock lock1;
	Lock::SinglenterLock lock2;
	mydata.calc_lock = &lock1;
	mydata.exit_lock = &lock2;
	mydata.calc_data = 0;
	mydata.end_count = 0;
	mydata.use_lock = false;
	for (int i = 0; i < 10; i++)
	{
		System::Thread test_thread(MyTestCalculationFunc);
		System::THREADID tid = test_thread.Run(&mydata);
		EXPECT_NE(0, tid); if (!tid) return;
	}
	while (mydata.end_count != 10)
		System::ThisThread::Sleep(100);
	EXPECT_GT(MyTestCalculationResult, mydata.calc_data);
	mydata.calc_data = 0;
	mydata.end_count = 0;
	mydata.use_lock = true;
	for (int i = 0; i < 10; i++)
	{
		System::Thread test_thread(MyTestCalculationFunc);
		System::THREADID tid = test_thread.Run(&mydata);
		EXPECT_NE(0, tid); if (!tid) return;
	}
	while (mydata.end_count != 10)
		System::ThisThread::Sleep(100);
	EXPECT_EQ(MyTestCalculationResult, mydata.calc_data);
}

PTest(RecursiveLock, TestCalculation)
{
	MyTestCalculationData mydata;
	Lock::RecursiveLock lock1;
	Lock::RecursiveLock lock2;
	mydata.calc_lock = &lock1;
	mydata.exit_lock = &lock2;
	mydata.calc_data = 0;
	mydata.end_count = 0;
	mydata.use_lock = false;
	for (int i = 0; i < 10; i++)
	{
		System::Thread test_thread(MyTestCalculationFunc);
		System::THREADID tid = test_thread.Run(&mydata);
		EXPECT_NE(0, tid); if (!tid) return;
	}
	while (mydata.end_count != 10)
		System::ThisThread::Sleep(100);
	EXPECT_GT(MyTestCalculationResult, mydata.calc_data);
	mydata.calc_data = 0;
	mydata.end_count = 0;
	mydata.use_lock = true;
	for (int i = 0; i < 10; i++)
	{
		System::Thread test_thread(MyTestCalculationFunc);
		System::THREADID tid = test_thread.Run(&mydata);
		EXPECT_NE(0, tid); if (!tid) return;
	}
	while (mydata.end_count != 10)
		System::ThisThread::Sleep(100);
	EXPECT_EQ(MyTestCalculationResult, mydata.calc_data);
}

PTest(SpinLock, TestCalculation)
{
	MyTestCalculationData mydata;
	Lock::SpinLock lock1;
	Lock::SpinLock lock2;
	mydata.calc_lock = &lock1;
	mydata.exit_lock = &lock2;
	mydata.calc_data = 0;
	mydata.end_count = 0;
	mydata.use_lock = false;
	for (int i = 0; i < 10; i++)
	{
		System::Thread test_thread(MyTestCalculationFunc);
		System::THREADID tid = test_thread.Run(&mydata);
		EXPECT_NE(0, tid); if (!tid) return;
	}
	while (mydata.end_count != 10)
		System::ThisThread::Sleep(100);
	EXPECT_GT(MyTestCalculationResult, mydata.calc_data);
	mydata.calc_data = 0;
	mydata.end_count = 0;
	mydata.use_lock = true;
	for (int i = 0; i < 10; i++)
	{
		System::Thread test_thread(MyTestCalculationFunc);
		System::THREADID tid = test_thread.Run(&mydata);
		EXPECT_NE(0, tid); if (!tid) return;
	}
	while (mydata.end_count != 10)
		System::ThisThread::Sleep(100);
	EXPECT_EQ(MyTestCalculationResult, mydata.calc_data);
}

PTest(SemaphoreInit11, TestCalculation)
{
	MyTestCalculationData mydata;
	Lock::Semaphore lock1(1,1);
	Lock::Semaphore lock2(1,1);
	mydata.calc_lock = &lock1;
	mydata.exit_lock = &lock2;
	mydata.calc_data = 0;
	mydata.end_count = 0;
	mydata.use_lock = false;
	for (int i = 0; i < 10; i++)
	{
		System::Thread test_thread(MyTestCalculationFunc);
		System::THREADID tid = test_thread.Run(&mydata);
		EXPECT_NE(0, tid); if (!tid) return;
	}
	while (mydata.end_count != 10)
		System::ThisThread::Sleep(100);
	EXPECT_GT(MyTestCalculationResult, mydata.calc_data);
	mydata.calc_data = 0;
	mydata.end_count = 0;
	mydata.use_lock = true;
	for (int i = 0; i < 10; i++)
	{
		System::Thread test_thread(MyTestCalculationFunc);
		System::THREADID tid = test_thread.Run(&mydata);
		EXPECT_NE(0, tid); if (!tid) return;
	}
	while (mydata.end_count != 10)
		System::ThisThread::Sleep(100);
	EXPECT_EQ(MyTestCalculationResult, mydata.calc_data);
}

PTest(AutoResetEventInitSet, TestCalculation)
{
	MyTestCalculationData mydata;
	Lock::AutoResetEvent lock1(True);
	Lock::AutoResetEvent lock2(True);
	mydata.calc_lock = &lock1;
	mydata.exit_lock = &lock2;
	mydata.calc_data = 0;
	mydata.end_count = 0;
	mydata.use_lock = false;
	for (int i = 0; i < 10; i++)
	{
		System::Thread test_thread(MyTestCalculationFunc);
		System::THREADID tid = test_thread.Run(&mydata);
		EXPECT_NE(0, tid); if (!tid) return;
	}
	while (mydata.end_count != 10)
		System::ThisThread::Sleep(100);
	EXPECT_GT(MyTestCalculationResult, mydata.calc_data);
	mydata.calc_data = 0;
	mydata.end_count = 0;
	mydata.use_lock = true;
	for (int i = 0; i < 10; i++)
	{
		System::Thread test_thread(MyTestCalculationFunc);
		System::THREADID tid = test_thread.Run(&mydata);
		EXPECT_NE(0, tid); if (!tid) return;
	}
	while (mydata.end_count != 10)
		System::ThisThread::Sleep(100);
	EXPECT_EQ(MyTestCalculationResult, mydata.calc_data);
}


/** test recursively enter lock */
typedef struct {
	Lock::ILock* test_lock;
	long test_result;
	int test_start;
} MyTestSinglenterData;

Void MyTestSinglenterFunc(Void* args)
{
	MyTestSinglenterData* pdata = (MyTestSinglenterData*)args;
	pdata->test_lock->Get();
	pdata->test_result += pdata->test_start;
	pdata->test_start--;
	if (pdata->test_start)
		MyTestSinglenterFunc(args);
	pdata->test_lock->Free();
}

PTest(SinglenterLock, TestSinglenter)
{
	Lock::SinglenterLock lock;
	MyTestSinglenterData mydata;
	mydata.test_lock = &lock;
	mydata.test_result = 0;
	mydata.test_start = 4;
	System::Thread test_thread(MyTestSinglenterFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_EQ(4, mydata.test_result);
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(50)))
		test_thread.Kill();
}

PTest(RecursiveLock, TestRecursively)
{
	Lock::RecursiveLock lock;
	MyTestSinglenterData mydata;
	mydata.test_lock = &lock;
	mydata.test_result = 0;
	mydata.test_start = 4;
	System::Thread test_thread(MyTestSinglenterFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_EQ(10, mydata.test_result);
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(50)))
		test_thread.Kill();
}

PTest(SpinLock, TestSinglenter)
{
	Lock::SpinLock lock;
	MyTestSinglenterData mydata;
	mydata.test_lock = &lock;
	mydata.test_result = 0;
	mydata.test_start = 4;
	System::Thread test_thread(MyTestSinglenterFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_EQ(4, mydata.test_result);
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(50)))
		test_thread.Kill();
}

PTest(Semaphore, TestSinglenter11)
{
	Lock::Semaphore lock(1,1);
	MyTestSinglenterData mydata;
	mydata.test_lock = &lock;
	mydata.test_result = 0;
	mydata.test_start = 4;
	System::Thread test_thread(MyTestSinglenterFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_EQ(4, mydata.test_result);
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(50)))
		test_thread.Kill();
}
PTest(Semaphore, TestSinglenter22)
{
	Lock::Semaphore lock(2, 2);
	MyTestSinglenterData mydata;
	mydata.test_lock = &lock;
	mydata.test_result = 0;
	mydata.test_start = 4;
	System::Thread test_thread(MyTestSinglenterFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_EQ(7, mydata.test_result);
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(50)))
		test_thread.Kill();
}
PTest(Semaphore, TestSinglenter34)
{
	Lock::Semaphore lock(3, 4);
	MyTestSinglenterData mydata;
	mydata.test_lock = &lock;
	mydata.test_result = 0;
	mydata.test_start = 4;
	System::Thread test_thread(MyTestSinglenterFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_EQ(9, mydata.test_result);
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(50)))
		test_thread.Kill();
}
PTest(Semaphore, TestSinglenter44)
{
	Lock::Semaphore lock(4, 4);
	MyTestSinglenterData mydata;
	mydata.test_lock = &lock;
	mydata.test_result = 0;
	mydata.test_start = 4;
	System::Thread test_thread(MyTestSinglenterFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_EQ(10, mydata.test_result);
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(50)))
		test_thread.Kill();
}

PTest(AutoResetEvent, TestSinglenterT)
{
	Lock::AutoResetEvent lock(True);
	MyTestSinglenterData mydata;
	mydata.test_lock = &lock;
	mydata.test_result = 0;
	mydata.test_start = 4;
	System::Thread test_thread(MyTestSinglenterFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_EQ(4, mydata.test_result);
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(50)))
		test_thread.Kill();
}
PTest(AutoResetEvent, TestSinglenterF)
{
	Lock::AutoResetEvent lock(False);
	MyTestSinglenterData mydata;
	mydata.test_lock = &lock;
	mydata.test_result = 0;
	mydata.test_start = 4;
	System::Thread test_thread(MyTestSinglenterFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_EQ(0, mydata.test_result);
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(50)))
		test_thread.Kill();
}

PTest(ManualResetEvent, TestSinglenterT)
{
	Lock::ManualResetEvent lock(True);
	MyTestSinglenterData mydata;
	mydata.test_lock = &lock;
	mydata.test_result = 0;
	mydata.test_start = 4;
	System::Thread test_thread(MyTestSinglenterFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_EQ(10, mydata.test_result);
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(50)))
		test_thread.Kill();
}
PTest(ManualResetEvent, TestSinglenterF)
{
	Lock::ManualResetEvent lock(False);
	MyTestSinglenterData mydata;
	mydata.test_lock = &lock;
	mydata.test_result = 0;
	mydata.test_start = 4;
	System::Thread test_thread(MyTestSinglenterFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (!tid) return;
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(250)));
	EXPECT_EQ(0, mydata.test_result);
	if (!test_thread.Wait(Time::TimeUnit::FromMilliseconds(50)))
		test_thread.Kill();
}


/** test waiting different time on one synchronization object **/
#define TMTWTCOUNT 6
#define TMTWINTVAL 200
typedef System::THREADSTARTINGENTRY TMTWFUNC;
typedef struct {
	Void* priv;
	Lock::ILock* wlock;
	TMTWFUNC test_func;
	Int32 total_count;
	Int32 passed_count;
	Int32 fail_threads;
	Bool success_case1;
	Bool success_case2;
} MyTestMultiTimeWaitOneData;

typedef struct {
	Void* priv;
	Lock::ILock* g_lock;
	System::Thread* l_pth;
	Lock::ILock* l_sync;
	UInt8 index;
	Bool result;
} MTMTWDPRIVDATA;

Void MyTestMultiTimeWaitOneFunc(Void* args)
{
	MTMTWDPRIVDATA tdata[TMTWTCOUNT];
	Lock::ManualResetEvent mre(False);
	MyTestMultiTimeWaitOneData* pdata = (MyTestMultiTimeWaitOneData*)args;

	int i;
	for (i = 0; i < TMTWTCOUNT; i++)
	{
		tdata[i].priv = pdata->priv;
		tdata[i].g_lock = pdata->wlock;
		tdata[i].l_sync = &mre;
		tdata[i].index = i+1;
		tdata[i].result = False;
		tdata[i].l_pth = new System::Thread(pdata->test_func);
		if (tdata[i].l_pth && 0 == tdata[i].l_pth->Run(&tdata[i]))
		{
			pdata->fail_threads++;
			delete tdata[i].l_pth;
			tdata[i].l_pth = Null;
		}
	}
	mre.Set();
	System::ThisThread::Sleep(TMTWINTVAL/4); // for condvar wait it into cond wait
	pdata->wlock->Free();
	System::ThisThread::Sleep(TMTWINTVAL/4);
	int n = 0;
	for (i = 0; i < TMTWTCOUNT; i++)
	{
		if (tdata[i].result)
			n++;
	}
	if (n == 1) pdata->success_case1 = True;
	pdata->wlock->Free();
	System::ThisThread::Sleep(TMTWINTVAL/4);
	n = 0;
	for (i = 0; i < TMTWTCOUNT; i++)
	{
		if (tdata[i].result)
			n++;
	}
	if (n == 2) pdata->success_case2 = True;
	System::ThisThread::Sleep(TMTWINTVAL);

	for (int i = 0; i < TMTWTCOUNT; i++)
	{
		if (tdata[i].result)
			pdata->passed_count++;
		if (tdata[i].l_pth)
		{
			if (tdata[i].l_pth->Status() == System::Thread::Running) {
				tdata[i].l_pth->Kill();
			}
			delete tdata[i].l_pth;
		}
		System::ThisThread::Sleep(TMTWINTVAL);
	}
}

Void TestMultiTimeWaitOne_Semaphore(Void* args)
{
	MTMTWDPRIVDATA* pdata = (MTMTWDPRIVDATA*)args;
	Lock::Semaphore* psem = reinterpret_cast<Lock::Semaphore*>(pdata->g_lock);
	pdata->l_sync->Get();
	pdata->l_sync->Free();
	psem->Wait(Time::TimeDelta::FromMilliseconds(pdata->index * TMTWINTVAL));
	pdata->result = True;
}

PTest(Semaphore, TestMultiTimeWaitOne)
{
	Lock::Semaphore semlock(0, Lock::Semaphore::MAXIMUM_COUNT);
	Int32 thds = 0;
	MyTestMultiTimeWaitOneData mydata;
	mydata.priv = Null;
	mydata.wlock = &semlock;
	mydata.test_func = TestMultiTimeWaitOne_Semaphore;
	mydata.total_count = TMTWTCOUNT;
	mydata.fail_threads = 0;
	mydata.passed_count = 0;
	mydata.success_case1 = False;
	mydata.success_case2 = False;
	System::Thread test_thread(MyTestMultiTimeWaitOneFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (0 == tid) return;
	test_thread.Wait(Time::TimeUnit::Infinite());
	EXPECT_EQ(mydata.total_count, mydata.passed_count);
	EXPECT_EQ(0, mydata.fail_threads);
	EXPECT_TRUE(mydata.success_case1);
	EXPECT_TRUE(mydata.success_case2);
}

Void TestMultiTimeWaitOne_AutoResetEvent(Void* args)
{
	MTMTWDPRIVDATA* pdata = (MTMTWDPRIVDATA*)args;
	Lock::AutoResetEvent* pare = reinterpret_cast<Lock::AutoResetEvent*>(pdata->g_lock);
	pdata->l_sync->Get();
	pdata->l_sync->Free();
	pare->Wait(Time::TimeDelta::FromMilliseconds(pdata->index * TMTWINTVAL));
	pdata->result = True;
}

PTest(AutoResetEvent, TestMultiTimeWaitOne)
{
	Lock::AutoResetEvent are(False);
	Int32 thds = 0;
	MyTestMultiTimeWaitOneData mydata;
	mydata.priv = Null;
	mydata.wlock = &are;
	mydata.test_func = TestMultiTimeWaitOne_AutoResetEvent;
	mydata.total_count = TMTWTCOUNT;
	mydata.fail_threads = 0;
	mydata.passed_count = 0;
	mydata.success_case1 = False;
	mydata.success_case2 = False;
	System::Thread test_thread(MyTestMultiTimeWaitOneFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (0 == tid) return;
	test_thread.Wait(Time::TimeUnit::Infinite());
	EXPECT_EQ(mydata.total_count, mydata.passed_count);
	EXPECT_EQ(0, mydata.fail_threads);
	EXPECT_TRUE(mydata.success_case1);
	EXPECT_TRUE(mydata.success_case2);
}

Void TestMultiTimeWaitOne_ConditionVariable(Void* args)
{
	MTMTWDPRIVDATA* pdata = (MTMTWDPRIVDATA*)args;
	Lock::ConditionVariable* pcond = reinterpret_cast<Lock::ConditionVariable*>(pdata->g_lock);
	Lock::ILock* plock = static_cast<Lock::ILock*>(pdata->priv);
	pdata->l_sync->Get();
	pdata->l_sync->Free();
	plock->Get();
	pcond->Wait(Time::TimeDelta::FromMilliseconds(pdata->index * TMTWINTVAL));
	pdata->result = True;
	plock->Free();
}

PTest(ConditionVariable, TestMultiTimeWaitOne, STANDALONE)
{
	Lock::Mutex mtx;
	Lock::ConditionVariable cond(&mtx);
	Int32 thds = 0;
	MyTestMultiTimeWaitOneData mydata;
	mydata.priv = &mtx;
	mydata.wlock = &cond;
	mydata.test_func = TestMultiTimeWaitOne_ConditionVariable;
	mydata.total_count = TMTWTCOUNT;
	mydata.fail_threads = 0;
	mydata.passed_count = 0;
	mydata.success_case1 = False;
	mydata.success_case2 = False;
	System::Thread test_thread(MyTestMultiTimeWaitOneFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid); if (0 == tid) return;
	test_thread.Wait(Time::TimeUnit::Infinite());
	EXPECT_EQ(mydata.total_count, mydata.passed_count);
	EXPECT_EQ(0, mydata.fail_threads);
	EXPECT_TRUE(mydata.success_case1);
	EXPECT_TRUE(mydata.success_case2);
}


#if __PLATFORM_OSYS_ != PTOS_WINDOWS
/** test tick clock timeout waiting be enabled */
#include <sys/time.h>
typedef System::THREADSTARTINGENTRY WITTFUNC;
typedef struct {
	Lock::ILock* g_sync;
	Lock::ILock* s_lock;
	System::Thread* pth;
	WITTFUNC test_func;
	Int32* thread_wptr;
	Int32* thread_eptr;
	Int32 test_threads;
	Int32 passed_count;
} MyTestWaitIsInTickTimeoutData;

typedef struct {
	Lock::ILock* g_sync;
	Lock::ILock* s_lock;
	System::Thread* pth;
	Int32* pwtr;
	Bool result;
} MTWIITTDPRIVDATA;

Void MyTestWaitIsInTickTimeoutFunc(Void* args)
{
	MyTestWaitIsInTickTimeoutData* pdata = (MyTestWaitIsInTickTimeoutData*)args;
	Lock::Mutex mlock;
	Int32 nsize = pdata->test_threads;
	Int32 nt_init = 0;
	Int32 nt_succ = 0;
	MTWIITTDPRIVDATA tests[nsize];
	Int32 i;
	for (i = 0; i<nsize; i++)
	{
		tests[i].g_sync = pdata->g_sync;
		tests[i].s_lock = &mlock;
		tests[i].result = False;
		tests[i].pwtr = &nt_init;
		tests[i].pth = new System::Thread(pdata->test_func);
		if (tests[i].pth && tests[i].pth->Run(&tests[i]))
			nt_succ++;
	}

	while (nt_init != nt_succ)
		System::ThisThread::Sleep(50);
	pdata->s_lock->Get();
	(*pdata->thread_wptr)++;
	pdata->s_lock->Free();
	pdata->g_sync->Get();
	pdata->g_sync->Free();

	System::ThisThread::Sleep(1000);
	for (i = 0; i<nsize; i++)
	{
		if (tests[i].result)
			pdata->passed_count++;
		if (!tests[i].pth)
			continue;
		if (tests[i].pth->Status() == System::Thread::Running) {
			tests[i].pth->Kill();
		}
		delete tests[i].pth;
	}
	pdata->s_lock->Get();
	(*pdata->thread_eptr)++;
	pdata->s_lock->Free();
}

Void TestWaitIsInTickTimeout_Sleep(Void* args)
{
	MTWIITTDPRIVDATA* pdata = (MTWIITTDPRIVDATA*)args;
	if (pdata->s_lock)
	{
		pdata->s_lock->Get();
		(*pdata->pwtr)++;
		pdata->s_lock->Free();
	}
	pdata->g_sync->Get();
	pdata->g_sync->Free();
	System::ThisThread::Sleep(0);

	Long time_before = Time::TickCount::NowMilliseconds();
	System::ThisThread::Sleep(750);
	Long tt = Time::TickCount::NowMilliseconds() - time_before;
	pdata->result = (tt > 500 && tt < 1000);
}

Void TestWaitIsInTickTimeout_Semaphore(Void* args)
{
	MTWIITTDPRIVDATA* pdata = (MTWIITTDPRIVDATA*)args;
	Lock::Semaphore semlock(0, 1);
	if (pdata->s_lock)
	{
		pdata->s_lock->Get();
		(*pdata->pwtr)++;
		pdata->s_lock->Free();
	}
	pdata->g_sync->Get();
	pdata->g_sync->Free();
	System::ThisThread::Sleep(0);

	Long time_before = Time::TickCount::NowMilliseconds();
	semlock.Wait(Time::TimeDelta::FromMilliseconds(750));
	Long tt = Time::TickCount::NowMilliseconds() - time_before;
	pdata->result = (tt > 500 && tt < 1000);
}

Void TestWaitIsInTickTimeout_AutoResetEvent(Void* args)
{
	MTWIITTDPRIVDATA* pdata = (MTWIITTDPRIVDATA*)args;
	Lock::AutoResetEvent mre(False);
	if (pdata->s_lock)
	{
		pdata->s_lock->Get();
		(*pdata->pwtr)++;
		pdata->s_lock->Free();
	}
	pdata->g_sync->Get();
	pdata->g_sync->Free();
	System::ThisThread::Sleep(0);

	Long time_before = Time::TickCount::NowMilliseconds();
	mre.Wait(Time::TimeDelta::FromMilliseconds(750));
	Long tt = Time::TickCount::NowMilliseconds() - time_before;
	pdata->result = (tt > 500 && tt < 1000);
}

Void TestWaitIsInTickTimeout_ManualResetEvent(Void* args)
{
	MTWIITTDPRIVDATA* pdata = (MTWIITTDPRIVDATA*)args;
	Lock::ManualResetEvent mre(False);
	if (pdata->s_lock)
	{
		pdata->s_lock->Get();
		(*pdata->pwtr)++;
		pdata->s_lock->Free();
	}
	pdata->g_sync->Get();
	pdata->g_sync->Free();
	System::ThisThread::Sleep(0);

	Long time_before = Time::TickCount::NowMilliseconds();
	mre.Wait(Time::TimeDelta::FromMilliseconds(750));
	Long tt = Time::TickCount::NowMilliseconds() - time_before;
	pdata->result = (tt > 500 && tt < 1000);
}

Void TestWaitIsInTickTimeout_ConditionVariable(Void* args)
{
	MTWIITTDPRIVDATA* pdata = (MTWIITTDPRIVDATA*)args;
	Lock::Mutex mtx;
	Lock::ConditionVariable cond(&mtx);
	mtx.Get();
	if (pdata->s_lock)
	{
		pdata->s_lock->Get();
		(*pdata->pwtr)++;
		pdata->s_lock->Free();
	}
	pdata->g_sync->Get();
	pdata->g_sync->Free();
	System::ThisThread::Sleep(0);

	Long time_before = Time::TickCount::NowMilliseconds();
	cond.Wait(Time::TimeDelta::FromMilliseconds(750));
	Long tt = Time::TickCount::NowMilliseconds() - time_before;
	pdata->result = (tt > 500 && tt < 1000);
	mtx.Free();
}

Void TestWaitIsInTickTimeout_ThreadSyncFunc(Void* args)
{
	System::ThisThread::Sleep(300000);
}

Void TestWaitIsInTickTimeout_ThreadSync(Void* args)
{
	MTWIITTDPRIVDATA* pdata = (MTWIITTDPRIVDATA*)args;
	System::Thread test_thread(TestWaitIsInTickTimeout_ThreadSyncFunc);
	test_thread.Run(0);
	if (pdata->s_lock)
	{
		pdata->s_lock->Get();
		(*pdata->pwtr)++;
		pdata->s_lock->Free();
	}
	pdata->g_sync->Get();
	pdata->g_sync->Free();
	System::ThisThread::Sleep(0);

	Long time_before = Time::TickCount::NowMilliseconds();
	test_thread.Wait(Time::TimeDelta::FromMilliseconds(750));
	Long tt = Time::TickCount::NowMilliseconds() - time_before;
	pdata->result = (tt > 500 && tt < 1000);
	test_thread.Kill();
}

PTest(All, TestWaitIsInTickTimeout, STANDALONE)
{
	Lock::ManualResetEvent mre(False);
	MTWIITTDPRIVDATA mydata = { &mre, Null, Null, Null, False };
	EXPECT_FALSE(mydata.result);
	System::Thread sleep_thread(TestWaitIsInTickTimeout_Sleep);
	System::THREADID sleep_tid = sleep_thread.Run(&mydata);
	EXPECT_NE(0, sleep_tid); if (!sleep_tid) return;
	System::ThisThread::Sleep(50);
	struct timeval tv;
	EXPECT_EQ(0, gettimeofday(&tv, NULL));
	tv.tv_sec -= 300;
	mre.Set();
	EXPECT_EQ(0, settimeofday(&tv, NULL));
	Bool sleep_ret = sleep_thread.Wait(Time::TimeUnit::FromMilliseconds(1000));
	EXPECT_TRUE(sleep_ret);
	EXPECT_TRUE(mydata.result);
	if (!sleep_ret) {
		sleep_thread.Kill();
		EXPECT_EQ(0, gettimeofday(&tv, NULL));
		tv.tv_sec += 300;
		EXPECT_EQ(0, settimeofday(&tv, NULL));
		return;
	}
	mre.Reset();
	int tt_nums = 100;
	Int32 nc_all = 0;
	Int32 nc_fin = 0;
	Int32 nc_end = 0;
	Lock::Mutex mtx;
	MyTestWaitIsInTickTimeoutData myds[] = {
		{ &mre, &mtx, Null, TestWaitIsInTickTimeout_Sleep, &nc_fin, &nc_end, tt_nums, 0 },
		{ &mre, &mtx, Null, TestWaitIsInTickTimeout_Semaphore, &nc_fin, &nc_end, tt_nums, 0 },
		{ &mre, &mtx, Null, TestWaitIsInTickTimeout_AutoResetEvent, &nc_fin, &nc_end, tt_nums, 0 },
		{ &mre, &mtx, Null, TestWaitIsInTickTimeout_ManualResetEvent, &nc_fin, &nc_end, tt_nums, 0 },
		{ &mre, &mtx, Null, TestWaitIsInTickTimeout_ConditionVariable, &nc_fin, &nc_end, tt_nums, 0 },
		{ &mre, &mtx, Null, TestWaitIsInTickTimeout_ThreadSync, &nc_fin, &nc_end, tt_nums, 0 }
	};
	for (int i = 0; i<sizeof(myds) / sizeof(myds[0]); i++)
	{
		EXPECT_FALSE(myds[i].passed_count);
		myds[i].pth = new System::Thread(MyTestWaitIsInTickTimeoutFunc);
		System::THREADID tid = myds[i].pth->Run(&myds[i]);
		EXPECT_NE(0, tid); if (!tid) return;
		nc_all++;
	}
	do {
		System::ThisThread::Sleep(50);
	} while (nc_fin != nc_all);
	mre.Set();
	System::ThisThread::Sleep(3);
	EXPECT_EQ(0, gettimeofday(&tv, NULL));
	tv.tv_sec -= 300;
	EXPECT_EQ(0, settimeofday(&tv, NULL));
	System::ThisThread::Sleep(1200);

	MyTestWaitIsInTickTimeoutData* Sleep_WaitTest = &myds[0];
	EXPECT_EQ(Sleep_WaitTest->test_threads, Sleep_WaitTest->passed_count);
	MyTestWaitIsInTickTimeoutData* Semaphore_WaitTest = &myds[1];
	EXPECT_EQ(Semaphore_WaitTest->test_threads, Semaphore_WaitTest->passed_count);
	MyTestWaitIsInTickTimeoutData* AutoEvent_WaitTest = &myds[2];
	EXPECT_EQ(AutoEvent_WaitTest->test_threads, AutoEvent_WaitTest->passed_count);
	MyTestWaitIsInTickTimeoutData* ManualEvent_WaitTest = &myds[3];
	EXPECT_EQ(ManualEvent_WaitTest->test_threads, ManualEvent_WaitTest->passed_count);
	MyTestWaitIsInTickTimeoutData* CondVariable_WaitTest = &myds[4];
	EXPECT_EQ(CondVariable_WaitTest->test_threads, CondVariable_WaitTest->passed_count);
	MyTestWaitIsInTickTimeoutData* ThreadSync_WaitTest = &myds[5];
	EXPECT_EQ(ThreadSync_WaitTest->test_threads, ThreadSync_WaitTest->passed_count);

	EXPECT_EQ(0, gettimeofday(&tv, NULL));
	tv.tv_sec += 600;
	EXPECT_EQ(0, settimeofday(&tv, NULL));
	while (nc_end != nc_all) {
		System::ThisThread::Sleep(50);
	}
	for (int i = 0; i<sizeof(myds) / sizeof(myds[0]); i++)
	{
		if (myds[i].pth)
			delete myds[i].pth;
	}
}
#endif

