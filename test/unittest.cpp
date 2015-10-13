#include "unittest.h"
#include <stdio.h>
#include <system/synchronization/lock.hh>
#include <system/synchronization/semaphore.hh>
#include <system/synchronization/event.hh>
#include <system/time/tickcount.hh>
#include <system/thread.hh>
#include <system/sysinfo.hh>
#include <system/debug.hh>

#if __PLATFORM_OSYS_ == PTOS_WINDOWS
#include <windows.h>
#else
#include <allocate.hh>
//#include <memops.hh>
#endif
#include <memops.hh>

#include <map>

namespace Prot {

	namespace{
		static System::STDFILE __sf_ = System::StdOut;
		static Int32(*__pf_)(const Char* fmt, ...) = System::Console::PrintF;
		PTU_TEST_CASE* _ifl_ = 0;
		PTU_TEST_CASE* _dfl_ = 0;
		static Bool _Inited_ = False;
	}

	PTU_TEST_ROOT UnitTest::_tr_ = {0,0,0};
	System::STDFILE UnitTest::_sf_ = System::StdOut;
	Int32(*UnitTest::_pf_)(const Char* fmt, ...) = System::Console::PrintF;
	UInt32 UnitTest::_nc_ = 0;
	UInt32 UnitTest::_fl_ = 0;


	Void UnitTest::SetOutput(System::STDFILE outdev)
	{
		if (outdev == System::StdIn)
			return;
		__sf_ = _sf_ = outdev;
		__pf_ = _pf_ = outdev == System::StdOut
							   ? System::Console::PrintF
							   : System::Console::EPrintF;
	}

#if __PLATFORM_OSYS_ == PTOS_WINDOWS
	HANDLE _g_heap_ = NULL;
	inline void _InitHeap_()
	{
		if (_g_heap_) return;
		_g_heap_ = HeapCreate(0, 0x10000, 0);
		if (!_g_heap_) Prot::System::Debug::Break();
	}
	inline void _FreeHeap_()
	{
		if (_g_heap_) HeapDestroy(_g_heap_);
	}
	inline AnyPtr Allocz(Size n)
	{
		return HeapAlloc(_g_heap_, HEAP_ZERO_MEMORY, n);
	}
	inline Void Freeaz(AnyPtr p)
	{
		HeapFree(_g_heap_, 0, p);
	}
#else
	inline void _InitHeap_(){}
	inline void _FreeHeap_(){}
	inline AnyPtr Allocz(Size n)
	{
		AnyPtr p = Memory::Alloc(n);
		Memory::Fill(p, n, 0);
		return p;
	}
	inline Void Freeaz(AnyPtr p)
	{
		Memory::Free(p);
	}
#endif
	Void UnitTest::InitTest()
	{
		if (_Inited_)
			return;
		_Inited_ = true;
		_InitHeap_();
		PTU_TEST_ROOT tr;
		tr.HeadCase = tr.LastCase = NULL;
		tr.TestCount = 0;
		_tr_ = tr;
	}
	Void UnitTest::DoneTest()
	{
		_FreeHeap_();
	}

	const char* UnitTest::AddTestCase(void(*tfunc)(PTU_TEST_CASE*), const char* c, const char* m, const char* n, int flags)
	{
		if (!_Inited_) InitTest();
		static bool test_meonly_enabled = false;
		if (!(flags & (UTEST_INIT | UTEST_DONE)))
		{
			if (test_meonly_enabled && !(flags & TESTMEONLY))
				return NULL;
			if (!test_meonly_enabled && (flags & TESTMEONLY))
			{
				PTU_TEST_CASE* p = _tr_.HeadCase;
				while (p)
				{
					PTU_TEST_CASE* t = p->Next;
					Freeaz(p);
					p = t;
				}
				_tr_.HeadCase = _tr_.LastCase = NULL;
				_nc_ = 0;
				test_meonly_enabled = true;
			}
		}
		PTU_TEST_CASE* p = (PTU_TEST_CASE*)Allocz(sizeof(PTU_TEST_CASE));
		p->ClassName = c;
		p->ModuleName = m;
		p->TestName = n;
		p->TestFunc = tfunc;
		p->TestResult = flags << 16;
		if (flags & (UTEST_INIT | UTEST_DONE))
		{
			PTU_TEST_CASE* pfl = (flags & UTEST_INIT) ? _ifl_ : _dfl_;
			if (pfl == NULL)
			{
				if (flags & UTEST_INIT)
					_ifl_ = p;
				else
					_dfl_ = p;
				return c;
			}
			do {
				if (0 == strcmp(pfl->ModuleName, m) && 0 == strcmp(pfl->TestName, n))
					return c;
				if (pfl->Next == NULL)
					break;
				pfl = pfl->Next;
			} while (1);
			pfl->Next = p;
			return c;
		}
		if (_tr_.LastCase)
		{
			_tr_.LastCase->Next = p;
		}
		else
		{
			_tr_.HeadCase = p;
		}
		_tr_.LastCase = p;
		_tr_.TestCount++;
		_nc_++;
		return c;
	}

	void UnitTest::PrintFailmem(const char* headstr, const char* exp, const void* val, int len)
	{
		char buf[80];
		unsigned char* pv = (unsigned char*)val;
		if (pv == NULL)
		{
			_pf_("%s %s is NULL\n", headstr, exp);
			return;
		}
		if (len < 16)
		{
			char* pb = buf;
			while (len--)
			{
				sprintf(pb, "%02X ", *pv++);
				pb += 3;
			}
			*pb = L'\0';
			if (pb != buf)
				*(pb - 1) = L'\0';
			_pf_("%s %s is [%s]\n", headstr, exp, buf);
		}
		else
		{
			int offset = 0;
			char* p = &buf[62];
			int explen = strlen(exp);
			if (explen > 72)
				explen = 72;
			memset(buf, '=', 80);
			buf[79] = L'\0';
			buf[3] = ' ';
			buf[4 + explen] = ' ';
			memcpy(&buf[4], exp, explen);
			_pf_("%s\n", buf);
			while (len >= 16)
			{
				sprintf(buf, " %08X: %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X %02X %02X | ",
					offset, pv[0], pv[1], pv[2], pv[3], pv[4], pv[5], pv[6], pv[7],
					pv[8], pv[9], pv[10], pv[11], pv[12], pv[13], pv[14], pv[15]);
				for (int i = 0; i < 16; i++)
				{
					if (pv[i] < ' ' || pv[i] > 0x7f)
						p[i] = '.';
					else
						p[i] = pv[i];
				}
				p[16] = L'\0';
				_pf_("%s\n", buf);
				len -= 16;
				offset += 16;
				pv += 16;
			}
			if (len)
			{
				sprintf(buf, " %08X:", offset);
				p = &buf[10];
				for (int i = 0; i < 16; i++)
				{
					if (i == 8)
						*p++ = ' ';
					if (i < len)
						sprintf(p, " %02X", pv[i]);
					else
						memcpy(p, "   ", 3);
					p += 3;
				}
				memcpy(p, " | ", 3);
				p += 3;
				for (int i = 0; i < 16; i++)
				{
					if (i == len)
					{
						p[i] = L'\0';
						break;
					}
					if (pv[i] < ' ' || pv[i] > 0x7f)
						p[i] = '.';
					else
						p[i] = pv[i];
				}
				_pf_("%s\n", buf);
			}
		}
	}

	/* return failed case */
	PTU_TEST_CASE* SingleThreadTest(PTU_TEST_CASE* ptestcase, int* pfailcount, Long* ptts)
	{
		Long tb, ta;
		PTU_TEST_CASE* ptest_top = ptestcase;
		while (ptestcase)
		{
			System::Console::CFPrintF(System::Console::Green, __sf_, "[ RUN      ]");
			__pf_(" %s.%s\n", ptestcase->ModuleName, ptestcase->TestName);
			tb = Time::TickCount::NowMilliseconds();
			ptestcase->TestFunc(ptestcase);
			ta = Time::TickCount::NowMilliseconds();
			ta -= tb;
			(*ptts) += ta;
			if (!(ptestcase->TestResult & PTU_TRF_IGNORE) && (ptestcase->FailureCount > 0))
				ptestcase->TestResult |= PTU_TRF_FAILURE;

			if (ptestcase->TestResult & PTU_TRF_FAILURE) {
				System::Console::CFPrintF(System::Console::Red, __sf_, "[  FAILED  ]");
				(*pfailcount)++;
			} else {
				System::Console::CFPrintF(System::Console::Green, __sf_, ptestcase->FailureCount > 0 ? "[  FAILED  ]" : "[       OK ]");
			}

			if (ptestcase->TotalCount)
				if (ptestcase->TestResult & PTU_TRF_IGNORE)
					System::Console::CFPrintF(ptestcase->FailureCount ? System::Console::Yellow : System::Console::DefaultText,
						__sf_, " %s.%s [%d/%d] (%1.3lf s) - IGNORE -\n", ptestcase->ModuleName, ptestcase->TestName, ptestcase->FailureCount, ptestcase->TotalCount, ta/(double)1000);
				else
					__pf_(" %s.%s [%d/%d] (%1.3lf s)\n", ptestcase->ModuleName, ptestcase->TestName, ptestcase->FailureCount, ptestcase->TotalCount, ta/(double)1000);
			else
				System::Console::CFPrintF(System::Console::DarkBlack, __sf_, " %s.%s -- NO TEST EXPRESSION EXIST --\n", ptestcase->ModuleName, ptestcase->TestName);

			if ((ptestcase->TestResult & PTU_TRF_BREAK) && (*pfailcount) != 0)
				break;
			ptestcase = ptestcase->Next;
		}
		return ptest_top;
	}

	typedef struct {
		Lock::Semaphore* stest_sem;
		Lock::ILock*	stest_lock;
		Lock::ILock*	scont_lock;		// console contend lock
		PTU_TEST_CASE*	ptest_case;
		PTU_TEST_CASE*	ptest_left;
		PTU_TEST_CASE*	ptest_fail;
		int*	pfail_count;
		Long*	pcount_tts;
		Int16	para_lines;
		Int16	test_flags;
	} PTUMTTDATA;

#define PTUMTTDATA_FLAG_BREAKOUT 1

	Void PtRunTestThreadProc(Void* args)
	{
		Long tb, ta;
		PTUMTTDATA* ptd = (PTUMTTDATA*)args;
		while (1)
		{
			ptd->stest_lock->Get();
			PTU_TEST_CASE* ptestcase = ptd->ptest_case;
			if (ptestcase)
				ptd->ptest_case = ptestcase->Next;
			ptd->stest_lock->Free();
			if (!ptestcase || (ptd->test_flags & PTUMTTDATA_FLAG_BREAKOUT))
				break;
			if (ptestcase->TestResult & PTU_TRF_STANDALONE)
			{
				ptd->stest_lock->Get();
				ptestcase->Next = ptd->ptest_left;
				ptd->ptest_left = ptestcase;
				ptd->stest_lock->Free();
				continue;
			}
			tb = Time::TickCount::NowMilliseconds();
			ptestcase->TestFunc(ptestcase);
			ta = Time::TickCount::NowMilliseconds();
			ta -= tb;

			ptd->scont_lock->Get();
			(*ptd->pcount_tts) += ta;
			//System::Console::CFPrintF(System::Console::Green, __sf_, "[ RUN      ]");
			//__pf_(" %s.%s\n", ptestcase->ModuleName, ptestcase->TestName);
			if (!(ptestcase->TestResult & PTU_TRF_IGNORE) && (ptestcase->FailureCount > 0))
			{
				ptestcase->TestResult |= PTU_TRF_FAILURE;
				ptestcase->Next = ptd->ptest_fail;
				ptd->ptest_fail = ptestcase;
			}

			if (ptestcase->TestResult & PTU_TRF_FAILURE) {
				System::Console::CFPrintF(System::Console::Red, __sf_, "[  FAILED  ]");
				(*ptd->pfail_count)++;
			} else {
				System::Console::CFPrintF(System::Console::Green, __sf_, ptestcase->FailureCount > 0 ? "[  FAILED  ]" : "[ RUN | OK ]");
			}
			if (ptestcase->TotalCount)
				if (ptestcase->TestResult & PTU_TRF_IGNORE)
					System::Console::CFPrintF(ptestcase->FailureCount ? System::Console::Yellow : System::Console::DefaultText,
					__sf_, " %s.%s [%d/%d] (%1.3lf s) - IGNORE -\n", ptestcase->ModuleName, ptestcase->TestName, ptestcase->FailureCount, ptestcase->TotalCount, ta / (double)1000);
				else
					__pf_(" %s.%s [%d/%d] (%1.3lf s)\n", ptestcase->ModuleName, ptestcase->TestName, ptestcase->FailureCount, ptestcase->TotalCount, ta / (double)1000);
			else
				System::Console::CFPrintF(System::Console::DarkBlack, __sf_, " %s.%s -- NO TEST EXPRESSION EXIST --\n", ptestcase->ModuleName, ptestcase->TestName);

			if ((ptestcase->TestResult & PTU_TRF_BREAK) && (*ptd->pfail_count) != 0)
			{
				ptd->test_flags |= PTUMTTDATA_FLAG_BREAKOUT;
			}
			ptd->scont_lock->Free();
		}
		ptd->stest_sem->Post();
	}

	PTU_TEST_CASE* MultiThreadTest(PTU_TEST_CASE* ptestcase, int* pfailcount, Long* ptts)
	{
		Int32 ncore_lines = System::Information::NumOfCPUCores();
		ncore_lines = ncore_lines > 0 ? ncore_lines*2 : 2;
		Int32 i;
		Lock::Semaphore semlock(0, ncore_lines);
		Lock::ManualResetEvent endevent(False);
		Lock::Mutex test_lock;
		Lock::Mutex cont_lock;
		PTUMTTDATA utd;
		utd.stest_sem = &semlock;
		utd.stest_lock = &test_lock;
		utd.scont_lock = &cont_lock;
		utd.ptest_case = ptestcase;
		utd.ptest_left = Null;
		utd.ptest_fail = Null;
		utd.pfail_count = pfailcount;
		utd.pcount_tts = ptts;
		utd.para_lines = ncore_lines;
		utd.test_flags = 0;
		for (i = 0; i < ncore_lines; i++)
		{
			System::Thread test_thread(PtRunTestThreadProc);
			if (!test_thread.Run(&utd))
				ncore_lines--;
		}
		if (ncore_lines == 0)
			return SingleThreadTest(ptestcase, pfailcount, ptts);
		i = 0;
		while (i < ncore_lines)
		{
			semlock.Wait(Time::TimeUnit::Infinite());
			i++;
		}
		utd.ptest_case = Null;
		if (!(utd.test_flags & PTUMTTDATA_FLAG_BREAKOUT))
		{
			/* re-arrange left standalone (single thread) cases */
			while (utd.ptest_left)
			{
				PTU_TEST_CASE* p = utd.ptest_left;
				utd.ptest_left = p->Next;
				p->Next = utd.ptest_case;
				utd.ptest_case = p;
			}
			utd.ptest_case = SingleThreadTest(utd.ptest_case, pfailcount, ptts);
		}
		/* collect failed case into _tr_.HeadCase */
		while (utd.ptest_fail)
		{
			PTU_TEST_CASE* p = utd.ptest_fail;
			utd.ptest_fail = p->Next;
			p->Next = utd.ptest_case;
			utd.ptest_case = p;
		}
		return utd.ptest_case;
	}

	int UnitTest::RunTest()
	{
		int rv = 0;
		PTU_TEST_CASE* p = _tr_.HeadCase;
		System::Console::CFPrintF(System::Console::Green, _sf_, "[==========] -- STARTUP : %d Cases Now Testing ...\n", _nc_);
		while (_ifl_)
		{
			System::Console::CFPrintF(System::Console::Green, __sf_, "[-- INIT --]");
			__pf_(" %s.%s\n", _ifl_->ModuleName, _ifl_->TestName);
			_ifl_->TestFunc(_ifl_);
			_ifl_ = _ifl_->Next;
		}
		Long tts = 0;
		Long ttb = Time::TickCount::NowMilliseconds();
		if ((_fl_ & PTUF_SINGLE_THREAD) || _nc_ < 2)
			_tr_.HeadCase = SingleThreadTest(p, &rv, &tts);
		else
			_tr_.HeadCase = MultiThreadTest(p, &rv, &tts);
		Long tta = Time::TickCount::NowMilliseconds();
		tta -= ttb;
		while (_dfl_)
		{
			System::Console::CFPrintF(System::Console::Green, __sf_, "[-- DONE --]");
			__pf_(" %s.%s\n", _dfl_->ModuleName, _dfl_->TestName);
			_dfl_->TestFunc(_dfl_);
			_dfl_ = _dfl_->Next;
		}
		if (rv == 0)
			System::Console::CFPrintF(System::Console::Green, _sf_, "[==========] -- SUMMARY : No Test Actually Failed.\n");
		else
		{
			int i = 0;
			System::Console::CFPrintF(System::Console::Red, _sf_, "[==========] -- SUMMARY : %d of %d tests are failed and list below.\n", rv, _tr_.TestCount);
			p = _tr_.HeadCase;
			while (p)
			{
				if (p->TestResult & PTU_TRF_FAILURE)
					System::Console::CFPrintF(System::Console::Red, _sf_, "[ %8d ] %s.%s\n", ++i, p->ModuleName, p->TestName);
				p = p->Next;
			}
		}
		_pf_("[     TIME ] -- SUMMARY : %1.3f - %1.3f = %1.3f (sec)\n", tta / (double)1000, tts / (double)1000, (tta - tts) / (double)1000);
		return -rv;
	}

	Void UnitTest::AddOption(const Char* key, const Char* val)
	{
		if (0 == Memory::Comp(key, "Threads", 8))
		{
			if (0 == Memory::Comp(val, "Single", 7))
				_fl_ |= PTUF_SINGLE_THREAD;
		}
	}
}

