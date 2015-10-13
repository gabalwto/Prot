#include <test/ptutest.h>
#include "condvar.hh"
#include <system/thread.hh>
#include <system/time/tickcount.hh>

/** test return value */
typedef struct {
	Lock::ILock* plock;
	Lock::ConditionVariable* pcond;
	Int32 count;
	Int32 result;
} MyCondVarTestReturnValueData;

Void MyCondVarTestReturnValueFunc(Void* args)
{
	MyCondVarTestReturnValueData* pdata = (MyCondVarTestReturnValueData*)args;
	if (pdata->plock)
		pdata->plock->Get();
	pdata->pcond->Wait(Time::TimeUnit::Infinite());
	pdata->result++;
	if (pdata->plock)
		pdata->plock->Free();
}

PTest(ConditionVariable, TestReturnValue)
{
	{
		Lock::ConditionVariable cond(Null);
		EXPECT_EQ(Lock::TypeConditionVariable, cond.Type());
		EXPECT_FALSE(cond.Wait(Time::TimeUnit::Zero()));
		EXPECT_EQ(0, cond.WakeAll());
		EXPECT_EQ(0, cond.WakeOne());

		MyCondVarTestReturnValueData mydata;
		mydata.plock = Null;
		mydata.pcond = &cond;
		mydata.count = 0;
		mydata.result = 0;

		for (int i = 0; i < 10; i++)
		{
			mydata.count++;
			System::Thread test_thread(MyCondVarTestReturnValueFunc);
			System::THREADID tid = test_thread.Run(&mydata);
			EXPECT_NE(0, tid); if (!tid) return;
		}
		System::ThisThread::Sleep(100);
		EXPECT_EQ(1, cond.WakeOne());
		System::ThisThread::Sleep(100);
		EXPECT_EQ(mydata.count - 1, cond.WakeAll());
		System::ThisThread::Sleep(100);
		EXPECT_EQ(0, cond.WakeOne());
		EXPECT_EQ(0, cond.WakeAll());
	}
	{
		Lock::Mutex lock;
		EXPECT_TRUE(lock.TryGet());
		EXPECT_FALSE(lock.TryGet());
		Lock::ConditionVariable cond(&lock);
		EXPECT_EQ(Lock::TypeConditionVariable, cond.Type());
		EXPECT_FALSE(cond.Wait(Time::TimeUnit::Zero()));
		EXPECT_FALSE(lock.TryGet());
		EXPECT_EQ(0, cond.WakeAll());
		EXPECT_EQ(0, cond.WakeOne());

		MyCondVarTestReturnValueData mydata;
		mydata.plock = &lock;
		mydata.pcond = &cond;
		mydata.count = 0;
		mydata.result = 0;

		lock.Free();
		for (int i = 0; i < 10; i++)
		{
			mydata.count++;
			System::Thread test_thread(MyCondVarTestReturnValueFunc);
			System::THREADID tid = test_thread.Run(&mydata);
			EXPECT_NE(0, tid); if (!tid) return;
		}
		System::ThisThread::Sleep(100);
		EXPECT_EQ(1, cond.WakeOne());
		System::ThisThread::Sleep(100);
		EXPECT_EQ(mydata.count - 1, cond.WakeAll());
		System::ThisThread::Sleep(100);
		EXPECT_EQ(0, cond.WakeOne());
		EXPECT_EQ(0, cond.WakeAll());
		EXPECT_EQ(mydata.count, mydata.result);
	}
}

/** test wake operation */
#include "semaphore.hh"
typedef struct {
	Lock::Semaphore* psem;
	Lock::ILock* plock;
	Lock::ConditionVariable* pcvin;
	Lock::ConditionVariable* pcvout;
	Int32 n_items;
	Int32 n_exitd;
	Int32 recv_cmd;
	Bool exit_cmd;
} MyCondVarTestWakeData;

typedef struct {
	MyCondVarTestWakeData* priv;
	Int32 fill_val;
	Int32 work_idx;
} MCVTWDITEM;

Void MyCondVarTestWakeAllFunc(Void* args)
{
	MCVTWDITEM* pitem = (MCVTWDITEM*)args;
	MyCondVarTestWakeData* pdata = (MyCondVarTestWakeData*)pitem->priv;
	pdata->plock->Get();
	pdata->psem->Post();
	while (pdata->recv_cmd != pitem->work_idx && !pdata->exit_cmd)
	{
		pdata->pcvin->Wait(Time::TimeUnit::Infinite());
	}
	pitem->fill_val = 1;
	pdata->recv_cmd = -1;
	pdata->pcvout->WakeAll();
	pdata->n_exitd++;
	pdata->plock->Free();
}

PTest(ConditionVariable, TestWakeAll)
{
	int i;
	Lock::Semaphore semlock(0, Lock::Semaphore::MAXIMUM_COUNT);
	Lock::Mutex lock;
	Lock::ConditionVariable condin(&lock);
	Lock::ConditionVariable condout(&lock);
	MyCondVarTestWakeData mydata;
	MCVTWDITEM myitem[60];
	mydata.psem = &semlock;
	mydata.plock = &lock;
	mydata.pcvin = &condin;
	mydata.pcvout = &condout;
	mydata.n_items = 0;
	mydata.n_exitd = 0;
	mydata.recv_cmd = -1;
	mydata.exit_cmd = False;
	for (i = 0; i < sizeof(myitem)/sizeof(myitem[0]); i++)
	{
		mydata.n_items++;
		myitem[i].priv = &mydata;
		myitem[i].work_idx = i;
		myitem[i].fill_val = 0;
		System::Thread test_thread(MyCondVarTestWakeAllFunc);
		System::THREADID tid = test_thread.Run(&myitem[i]);
		EXPECT_NE(0, tid); if (!tid) return;
	}
	i = 0;
	while (i < mydata.n_items)
	{
		semlock.Wait(Time::TimeUnit::Infinite());
		i++;
	}
	for (i = 0; i < mydata.n_items; i++)
	{
		EXPECT_EQ(0, myitem[i].fill_val);
	}
	for (i = 0; i < mydata.n_items; i++)
	{
		lock.Get();
		mydata.recv_cmd = i;
		condin.WakeAll();
		while (mydata.recv_cmd != -1)
		{
			condout.Wait(Time::TimeUnit::Infinite());
		}
		for (int j = 0; j < mydata.n_items; j++)
		{
			EXPECT_EQ(j > i ? 0 : 1, myitem[j].fill_val);
		}
		lock.Free();
	}
	EXPECT_EQ(mydata.n_items, mydata.n_exitd);
	mydata.exit_cmd = True;
	condin.WakeAll();
}

Void MyCondVarTestWakeOneFunc(Void* args)
{
	MCVTWDITEM* pitem = (MCVTWDITEM*)args;
	MyCondVarTestWakeData* pdata = (MyCondVarTestWakeData*)pitem->priv;
	pdata->plock->Get();
	pdata->psem->Post();
	while (pdata->recv_cmd < 0 && !pdata->exit_cmd)
	{
		pdata->pcvin->Wait(Time::TimeUnit::Infinite());
	}
	pitem->fill_val = pdata->recv_cmd;
	pdata->recv_cmd = -1;
	pdata->pcvout->WakeAll();
	pdata->n_exitd++;
	pdata->plock->Free();
}

PTest(ConditionVariable, TestWakeOne)
{
	int i,j,n;
	Lock::Semaphore semlock(0, Lock::Semaphore::MAXIMUM_COUNT);
	Lock::Mutex lock;
	Lock::ConditionVariable condin(&lock);
	Lock::ConditionVariable condout(&lock);
	MyCondVarTestWakeData mydata;
	MCVTWDITEM myitem[120];
	mydata.psem = &semlock;
	mydata.plock = &lock;
	mydata.pcvin = &condin;
	mydata.pcvout = &condout;
	mydata.n_items = 0;
	mydata.n_exitd = 0;
	mydata.recv_cmd = -1;
	mydata.exit_cmd = False;
	for (i = 0; i < sizeof(myitem)/sizeof(myitem[0]); i++)
	{
		mydata.n_items++;
		myitem[i].priv = &mydata;
		myitem[i].work_idx = i;
		myitem[i].fill_val = -1;
		System::Thread test_thread(MyCondVarTestWakeOneFunc);
		System::THREADID tid = test_thread.Run(&myitem[i]);
		EXPECT_NE(0, tid); if (!tid) return;
	}
	i = 0;
	while (i < mydata.n_items)
	{
		semlock.Wait(Time::TimeUnit::Infinite());
		i++;
	}
	for (i = 0; i < mydata.n_items; i++)
	{
		EXPECT_EQ(-1, myitem[i].fill_val);
	}
	for (i = 0; i < mydata.n_items; i++)
	{
		lock.Get();
		mydata.recv_cmd = i;
		condin.WakeOne();
		while (mydata.recv_cmd != -1)
		{
			condout.Wait(Time::TimeUnit::Infinite());
		}
		n = 0;
		for (j = 0; j < mydata.n_items; j++)
		{
			if (myitem[j].fill_val != -1)
				n++;
		}
		lock.Free();
		EXPECT_EQ(i + 1, n);
	}
	EXPECT_EQ(mydata.n_items, mydata.n_exitd);
	n = 0;
	for (i = 0; i < mydata.n_items; i++)
	{
		n += myitem[i].fill_val;
	}
	EXPECT_EQ(mydata.n_items*(mydata.n_items-1)/2, n);
	mydata.exit_cmd = True;
	condin.WakeAll();
}


typedef struct {
	Lock::Semaphore* psem;
	Lock::ILock* plock;
	Lock::ConditionVariable* pcdin;
	Lock::ConditionVariable* pcdout;
	Int32 maxcount;
	Int32 fill_idx;
	Int32 temp_idx;
	Int32* pfill;
} MyTestOrderedReturnData;

Void MyTestOrderedReturnFunc(Void* args)
{
	MyTestOrderedReturnData* pdata = (MyTestOrderedReturnData*)args;
	pdata->plock->Get();
	Int32 idx = pdata->temp_idx++;
	pdata->psem->Post();
	while (pdata->fill_idx < 0)
	{
		pdata->pcdin->Wait(Time::TimeUnit::Infinite());
	}
	pdata->pfill[idx] = pdata->fill_idx;
	pdata->fill_idx = -1;
	pdata->pcdout->WakeAll();
	pdata->plock->Free();
}

Void TestOrderedReturn_ProduceFunc(Void* args)
{
	MyTestOrderedReturnData* pdata = (MyTestOrderedReturnData*)args;
	int n = pdata->maxcount >> 4;
	for (int i = 0; i < n; i++)
	{
		System::Thread test_thread(MyTestOrderedReturnFunc);
		System::THREADID tid = test_thread.Run(args);
	}
	pdata->psem->Post();
}

PTest(ConditionVariable, TestOrderedReturn)
{
	Int32 fill_buf[1000];
	Lock::Semaphore semlock(0, Lock::Semaphore::MAXIMUM_COUNT);
	Lock::Mutex lock;
	Lock::ConditionVariable condin(&lock);
	Lock::ConditionVariable condout(&lock);
	MyTestOrderedReturnData mydata;
	int i, unordered_count;
	mydata.psem = &semlock;
	mydata.plock = &lock;
	mydata.pcdin = &condin;
	mydata.pcdout = &condout;
	mydata.fill_idx = -1;
	mydata.temp_idx = 0;
	mydata.pfill = fill_buf;
	mydata.maxcount = (sizeof(fill_buf) / sizeof(fill_buf[0])) & 0xfffffff0;
	lock.Get();
	for (i = 0; i < 16; i++)
	{
		System::Thread test_thread(TestOrderedReturn_ProduceFunc);
		System::THREADID tid = test_thread.Run(&mydata);
		EXPECT_NE(0, tid); if (!tid) return;
	}
	i = 0;
	while (i < 16)
	{
		semlock.Wait(Time::TimeUnit::Infinite());
		i++;
	}
	lock.Free();
	i = 0;
	while (i < mydata.maxcount)
	{
		semlock.Wait(Time::TimeUnit::Infinite());
		i++;
	}
	System::ThisThread::Sleep(10);
	lock.Get();
	for (i = 0; i < mydata.maxcount; i++)
	{
		mydata.fill_idx = i;
		condin.WakeOne();
		while (mydata.fill_idx != -1)
		{
			condout.Wait(Time::TimeUnit::Infinite());
		}
	}
	lock.Free();
	unordered_count = 0;
	for (i = 0; i < mydata.maxcount; i++)
	{
		EXPECT_EQ(i, fill_buf[i]);
		if (fill_buf[i] != i)
			unordered_count++;
	}
	EXPECT_EQ(0, unordered_count);
	T_IGNORE();
}
