#include <windows.h>

#include "allocate.hh"
#include "memops.hh"

namespace Prot {
namespace Lock {

	SinglenterLock::SinglenterLock()
	{
		_syo = CreateEvent(NULL, FALSE, TRUE, NULL);
	}

	SinglenterLock::~SinglenterLock()
	{
		if (_syo) CloseHandle(_syo);
	}

	LockType SinglenterLock::Type()
	{
		return TypeSinglenter;
	}

	Bool SinglenterLock::Get()
	{
		return WaitForSingleObject(_syo, INFINITE) == WAIT_OBJECT_0;
	}

	Void SinglenterLock::Free()
	{
		SetEvent(_syo);
	}

	Bool SinglenterLock::TryGet()
	{
		return WaitForSingleObject(_syo, 0) == WAIT_OBJECT_0;
	}

	/**************************** RecursiveLock ******************************/
	RecursiveLock::RecursiveLock()
	{
		_syo = Memory::Alloc(sizeof(CRITICAL_SECTION));
		InitializeCriticalSectionAndSpinCount(_syo, 2000);
	}

	RecursiveLock::~RecursiveLock()
	{
		DeleteCriticalSection(_syo);
		Memory::Free(_syo);
	}

	LockType RecursiveLock::Type()
	{
		return TypeRecursive;
	}

	Bool RecursiveLock::Get()
	{
		EnterCriticalSection(_syo);
		return true;
	}

	Void RecursiveLock::Free()
	{
		LeaveCriticalSection(_syo);
	}

	Bool RecursiveLock::TryGet()
	{
		return FALSE != TryEnterCriticalSection(_syo);
	}

	/**************************** SpinLock ******************************/
	SpinLock::SpinLock()
	{
		_syo = NullPtr;
	}

	LockType SpinLock::Type()
	{
		return TypeSpinning;
	}

	Bool SpinLock::Get()
	{
		LONG* psyo = (LONG*)&_syo;
#if (PLATFORM_BITS==32)
		HMODULE hKernel = GetModuleHandleA("kernel32.dll");
		FARPROC lpSleep = GetProcAddress(hKernel, "Sleep");
		__asm {
			jmp _Start;
		_Loop:
			mov ecx, lpSleep;
			jecxz _Start;
			push 1;
			call ecx;
		_Start:
			xor eax, eax;
			mov ecx, psyo;
			inc eax;
			xchg eax, [ecx];
			or eax, eax;
			jnz _Loop;
		}
#else
		while (1)
		{
			Int32 osyo = InterlockedExchange(psyo, 1);
			if (osyo == 0)
				break;
			Sleep(1);
		}
#endif
		return true;
	}

	Void SpinLock::Free()
	{
		LONG* psyo = (LONG*)&_syo;
#if (PLATFORM_BITS==32)
		__asm {
			xor eax, eax;
			mov ecx, psyo;
			xchg eax, [ecx];
		}
#else
		InterlockedExchange(psyo, 0);
#endif
	}

	Bool SpinLock::TryGet()
	{
		Bool Ret;
		LONG* psyo = (LONG*)&_syo;
#if (PLATFORM_BITS==32)
		__asm {
			xor eax, eax;
			mov ecx, psyo;
			inc eax;
			xchg eax, [ecx];
			or  eax, eax;
			sete Ret;
		}
#else
		Ret = (0 == InterlockedExchange(psyo, 1));
#endif
		return Ret;
	}

}
}