#include <test/ptutest.h>
#include "thread.hh"
#include "pthread.hh"
#include <system/time/tickcount.hh>
#include <system/time/timestamp.hh>
#include <memops.hh>

/** test this thread module value */
PTest(ThisThread, TestSleep)
{
	EXPECT_NE(0, System::ThisThread::GetID());
	Long time_before1 = Time::TickCount::NowMilliseconds();
	Long time_before2 = Time::TimeStamp::NowMilliseconds();
	System::ThisThread::Sleep(500);
	Long time_after1 = Time::TickCount::NowMilliseconds();
	Long time_after2 = Time::TimeStamp::NowMilliseconds();
	Long time_delta1 = time_after1 - time_before1;
	Long time_delta2 = time_after2 - time_before2;
	EXPECT_LT(250, time_delta1);
	EXPECT_GT(750, time_delta1);
	EXPECT_LT(250, time_delta2);
	EXPECT_GT(750, time_delta2);
}

Void MyTestThreadEnableFunc(Void* args)
{
	System::THREADID* p = (System::THREADID*)args;
	System::ThisThread::Sleep(400);
	*p = System::ThisThread::GetID();
}

/** test thread module value */
PTest(Thread, TestEnable)
{
	System::THREADID sTid = 0;
	System::Thread test_thread(MyTestThreadEnableFunc);
	Long time_before = Time::TickCount::NowMilliseconds();
	System::THREADID nTid = test_thread.Run(&sTid);
	EXPECT_NE(0, nTid);
	EXPECT_EQ(System::Thread::Running, test_thread.Status());
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(1000)));
	Long time_after = Time::TickCount::NowMilliseconds();
	EXPECT_EQ(System::Thread::Offline, test_thread.Status());
	EXPECT_EQ(nTid, sTid);
	EXPECT_LT(200, time_after - time_before);
	EXPECT_GT(600, time_after - time_before);
}

PTest(PoolThread, TestEnable)
{
	System::THREADID sTid = 0;
	System::PoolThread test_thread(MyTestThreadEnableFunc);
	Long time_before = Time::TickCount::NowMilliseconds();
	System::THREADID nTid = test_thread.Run(&sTid);
	EXPECT_NE(0, nTid);
	EXPECT_EQ(System::PoolThread::Running, test_thread.Status());
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(1000)));
	Long time_after = Time::TickCount::NowMilliseconds();
	EXPECT_EQ(System::PoolThread::Holding, test_thread.Status());
	EXPECT_EQ(nTid, sTid);
	EXPECT_LT(200, time_after - time_before);
	EXPECT_GT(600, time_after - time_before);
}


Void MyTestThreadKillFunc(Void* args)
{
	int* p = (int*)args;
	System::ThisThread::Sleep(400);
	*p = 1;
	System::ThisThread::Sleep(400);
	*p = 2;
}

PTest(Thread, TestKillAndStatus)
{
	int num = 0;
	System::Thread test_thread(MyTestThreadKillFunc);
	EXPECT_EQ(System::Thread::Offline, test_thread.Status());
	EXPECT_NE(0, test_thread.Run(&num));
	EXPECT_EQ(System::Thread::Running, test_thread.Status());
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(200)));
	EXPECT_EQ(System::Thread::Running, test_thread.Status());
	EXPECT_EQ(0, num);
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(400)));
	EXPECT_EQ(System::Thread::Running, test_thread.Status());
	EXPECT_EQ(1, num);
	EXPECT_TRUE(test_thread.Kill());
	EXPECT_EQ(System::Thread::Offline, test_thread.Status());
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(400)));
	EXPECT_EQ(1, num);
}

PTest(PoolThread, TestKillAndStatus)
{
	int num = 0;
	System::PoolThread test_thread(MyTestThreadKillFunc);
	EXPECT_EQ(System::PoolThread::Offline, test_thread.Status());
	EXPECT_NE(0, test_thread.Run(&num));
	EXPECT_EQ(System::PoolThread::Running, test_thread.Status());
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(200)));
	EXPECT_EQ(System::PoolThread::Running, test_thread.Status());
	EXPECT_EQ(0, num);
	EXPECT_FALSE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(400)));
	EXPECT_EQ(System::PoolThread::Running, test_thread.Status());
	EXPECT_EQ(1, num);
	EXPECT_TRUE(test_thread.Kill());
	EXPECT_EQ(System::PoolThread::Offline, test_thread.Status());
	EXPECT_TRUE(test_thread.Wait(Time::TimeUnit::FromMilliseconds(400)));
	EXPECT_EQ(1, num);
}

PTest(Thread, TestRunTwice)
{
	static int num = 0;
	System::Thread test_thread1(MyTestThreadKillFunc);
	System::Thread test_thread2(MyTestThreadKillFunc);
	System::THREADID tid1 = test_thread1.Run(&num);
	System::THREADID tid2 = test_thread2.Run(&num);
	EXPECT_NE(0, tid1);
	EXPECT_NE(0, tid2);
	EXPECT_NE(tid1, tid2);
	System::ThisThread::Sleep(600);
	System::THREADID tid = test_thread1.Run(&num);
	EXPECT_EQ(0, tid);
	System::ThisThread::Sleep(400);
	tid = test_thread1.Run(&num);
	EXPECT_NE(0, tid);
	EXPECT_NE(tid1, tid);
	EXPECT_NE(tid2, tid);
}

PTest(PoolThread, TestRunTwice)
{
	static int num = 0;
	System::PoolThread test_thread1(MyTestThreadKillFunc);
	System::PoolThread test_thread2(MyTestThreadKillFunc);
	System::THREADID tid1 = test_thread1.Run(&num);
	System::THREADID tid2 = test_thread2.Run(&num);
	EXPECT_NE(0, tid1);
	EXPECT_NE(0, tid2);
	EXPECT_NE(tid1, tid2);
	System::ThisThread::Sleep(600);
	System::THREADID tid = test_thread1.Run(&num);
	EXPECT_EQ(0, tid);
	System::ThisThread::Sleep(400);
	tid = test_thread1.Run(&num);
	EXPECT_EQ(tid1, tid);
}


#include <system/synchronization/lock.hh>
typedef struct {
	Lock::ILock* lock;
	Int32* count_ptr;
	Int8* array_ptr;
	Int32 count_max;
} MyTestMaxCreationData;

Void MyTestMaxCreationFunc(Void* args)
{
	MyTestMaxCreationData* pdata = (MyTestMaxCreationData*)args;
	pdata->lock->Get();
	Int32 n = *pdata->count_ptr;
	if (n < pdata->count_max)
		(*pdata->count_ptr)++;
	pdata->lock->Free();
	if (n >= pdata->count_max)
		return;
	if (n + 1 < pdata->count_max)
	{
		System::Thread test_thread(MyTestMaxCreationFunc);
		test_thread.Run(args);
	}

	Int8* p = &pdata->array_ptr[n];
	while(*p == 0)
		System::ThisThread::Sleep(100);
	*p = 2;
}

PTest(Thread, TestMaxCreation, STANDALONE)
{
	const Int32 nc_nums = 10000;
	Lock::Mutex lock;
	Int8 nt_arrs[nc_nums];
	Int8 nt_cpys[nc_nums];
	Int32 n_runtime_max_threads = 0;
	Memory::Fill(nt_arrs, nc_nums, 0);
	MyTestMaxCreationData mydata;
	mydata.lock = &lock;
	mydata.count_ptr = &n_runtime_max_threads;
	mydata.array_ptr = nt_arrs;
	mydata.count_max = nc_nums;
	System::Thread test_thread(MyTestMaxCreationFunc);
	System::THREADID tid = test_thread.Run(&mydata);
	EXPECT_NE(0, tid);
	while (1) {
		Int32 tmp = n_runtime_max_threads;
		System::ThisThread::Sleep(500);
		if (tmp == n_runtime_max_threads)
			break;
	}
	EXPECT_LT(5000, n_runtime_max_threads);
	Memory::Fill(nt_arrs, n_runtime_max_threads, 1);
	Memory::Fill(nt_cpys, n_runtime_max_threads, 2);

	int n_count = 0;
	while (n_count < 10) {
		System::ThisThread::Sleep(500);
		if (0 == Memory::Comp(nt_cpys, nt_arrs, n_runtime_max_threads))
			break;
		n_count++;
	}
	EXPECT_MEMEQ(nt_cpys, nt_arrs, n_runtime_max_threads);
}


#include <system/synchronization/semaphore.hh>
typedef struct {
	Lock::ILock* sum_lock;
	Lock::ILock* queue_lock;
	Lock::Semaphore* notify;
	System::PoolThread* thd;
	Int64* result_ptr;
	Int64 num_start;
	Int64 num_end;
	Int8 queue_id;
	Bool idle_now;
} MyTestPoolParallelCalcData;

typedef struct _MYTESTPOOLPARALLELCALCLINK {
	MyTestPoolParallelCalcData* data;
	struct _MYTESTPOOLPARALLELCALCLINK* next;
} MYTESTPOOLPARALLELCALCLINK;

static MYTESTPOOLPARALLELCALCLINK* MyTestPoolParallelCalcLinkHead = NULL;
static MYTESTPOOLPARALLELCALCLINK* MyTestPoolParallelCalcLinkLast = NULL;

Void MyTestPoolParallelCalcFunc(Void* args)
{
	MyTestPoolParallelCalcData* pdata = (MyTestPoolParallelCalcData*)args;
	Int64 result = 0;
	for (Int64 i = pdata->num_start; i < pdata->num_end; i++)
		result += i;
	pdata->sum_lock->Get();
	(*pdata->result_ptr) += result;
	pdata->sum_lock->Free();
	MYTESTPOOLPARALLELCALCLINK* p = new MYTESTPOOLPARALLELCALCLINK;
	p->data = pdata;
	p->next = Null;
	pdata->queue_lock->Get();
	if (MyTestPoolParallelCalcLinkHead == NULL)
		MyTestPoolParallelCalcLinkHead = p;
	else
		MyTestPoolParallelCalcLinkLast->next = p;
	MyTestPoolParallelCalcLinkLast = p;
	pdata->queue_lock->Free();
	pdata->idle_now = True;
	pdata->notify->Post();
}

PTest(PoolThread, TestPoolParallelCalc)
{
	Int32 num_of_lines = 8;	// may use CPU cores * 2 ?
	MyTestPoolParallelCalcData* pdata = new MyTestPoolParallelCalcData[num_of_lines];
	Lock::Semaphore semlock(num_of_lines, num_of_lines);
	Lock::Mutex sumlock;
	Lock::Mutex quelock;
	Int64 sum_from = 10000;
	Int64 sum_end  = 10000000000LL;
	Int32 sum_step = 100000000;
	Int64 result = 0;
	for (int i = 0; i < num_of_lines; i++)
	{
		pdata[i].sum_lock = &sumlock;
		pdata[i].queue_lock = &quelock;
		pdata[i].notify = &semlock;
		pdata[i].result_ptr = &result;
		pdata[i].idle_now = True;
		pdata[i].queue_id = i;
		pdata[i].thd = new System::PoolThread(MyTestPoolParallelCalcFunc);
		MYTESTPOOLPARALLELCALCLINK* p = new MYTESTPOOLPARALLELCALCLINK;
		p->data = &pdata[i];
		p->next = Null;
		if (MyTestPoolParallelCalcLinkHead == Null)
			MyTestPoolParallelCalcLinkHead = p;
		else
			MyTestPoolParallelCalcLinkLast->next = p;
		MyTestPoolParallelCalcLinkLast = p;
	}
	Int64 sum_pt = sum_from;
	Int64 sum_endp = sum_end + 1;
	while (sum_pt < sum_endp)
	{
		semlock.Wait(Time::TimeUnit::Infinite());
		EXPECT_NE(Null, MyTestPoolParallelCalcLinkHead);
		quelock.Get();
		MYTESTPOOLPARALLELCALCLINK* p = MyTestPoolParallelCalcLinkHead;
		MyTestPoolParallelCalcLinkHead = p->next;
		quelock.Free();
		MyTestPoolParallelCalcData* d = p->data;
		EXPECT_TRUE(d->idle_now);
		d->idle_now = False;
		d->num_start = sum_pt;
		if (sum_pt + sum_step < sum_endp)
			sum_pt += sum_step;
		else
			sum_pt = sum_endp;
		d->num_end = sum_pt;
		d->thd->Wait(Time::TimeUnit::Infinite());
		System::THREADID tid = d->thd->Run(d);
		EXPECT_NE(0, tid);
		delete p;
		if (!tid) break;
	}
	Int32 n = 0;
	Int64 sum_pair1 = sum_from + sum_end;
	Int64 sum_pair2 = sum_endp - sum_from;
	if (sum_pair1 & 1)
		sum_pair2 /= 2;
	else
		sum_pair1 /= 2;
	Int64 sum_result = sum_pair1 * sum_pair2;
	while (n < num_of_lines)
	{
		semlock.Wait(Time::TimeUnit::Infinite());
		EXPECT_NE(Null, MyTestPoolParallelCalcLinkHead);
		quelock.Get();
		MYTESTPOOLPARALLELCALCLINK* p = MyTestPoolParallelCalcLinkHead;
		MyTestPoolParallelCalcLinkHead = p->next;
		quelock.Free();
		MyTestPoolParallelCalcData* d = p->data;
		EXPECT_TRUE(d->idle_now);
		d->idle_now = False;
		delete d->thd;
		delete p;
		n++;
	}
	EXPECT_EQ(sum_result, result);

	delete[] pdata;
}
