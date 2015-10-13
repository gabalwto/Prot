#include <pthread.h>

#include "allocate.hh"
#include "memops.hh"

namespace Prot {
namespace Lock {

	SinglenterLock::SinglenterLock()
	{
		_syo = Memory::Alloc(sizeof(pthread_mutex_t));
		pthread_mutexattr_t mtxa;
		pthread_mutexattr_init(&mtxa);
		pthread_mutexattr_settype(&mtxa, PTHREAD_MUTEX_NORMAL);
		pthread_mutex_init(_syo, &mtxa);
		pthread_mutexattr_destroy(&mtxa);
	}

	SinglenterLock::~SinglenterLock()
	{
		if (_syo){
			pthread_mutex_destroy(_syo);
			Memory::Free(_syo);
		}
	}

	LockType SinglenterLock::Type()
	{
		return TypeSinglenter;
	}

	Bool SinglenterLock::Get()
	{
		return 0==pthread_mutex_lock(_syo);
	}

	Void SinglenterLock::Free()
	{
		pthread_mutex_unlock(_syo);
	}

	Bool SinglenterLock::TryGet()
	{
		return 0==pthread_mutex_trylock(_syo);		
	}

	/**************************** RecursiveLock ******************************/
	RecursiveLock::RecursiveLock()
	{
		_syo = Memory::Alloc(sizeof(pthread_mutex_t));
		pthread_mutexattr_t mtxa;
		pthread_mutexattr_init(&mtxa);
		pthread_mutexattr_settype(&mtxa, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(_syo, &mtxa);
		pthread_mutexattr_destroy(&mtxa);
	}

	RecursiveLock::~RecursiveLock()
	{
		if (_syo){
			pthread_mutex_destroy(_syo);
			Memory::Free(_syo);
		}
	}

	LockType RecursiveLock::Type()
	{
		return TypeRecursive;
	}

	Bool RecursiveLock::Get()
	{
		return 0==pthread_mutex_lock(_syo);
	}

	Void RecursiveLock::Free()
	{
		pthread_mutex_unlock(_syo);
	}

	Bool RecursiveLock::TryGet()
	{
		return 0==pthread_mutex_trylock(_syo);		
	}

	/**************************** SpinLock ******************************/
#include <stdio.h>
#include <unistd.h>

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
#define _SPIN_LOCK_USE_ASM_ 1
#endif

	SpinLock::SpinLock()
	{
#ifdef _SPIN_LOCK_USE_ASM_
		_syo = NullPtr;
#else
		_syo = Memory::Alloc(sizeof(pthread_spinlock_t));
		pthread_spin_init((pthread_spinlock_t*)_syo, PTHREAD_PROCESS_PRIVATE);
#endif
	}

	SpinLock::~SpinLock()
	{
#ifndef _SPIN_LOCK_USE_ASM_
		pthread_spin_destroy((pthread_spinlock_t*)_syo);
#endif
    }

	LockType SpinLock::Type()
	{
		return TypeSpinning;
	}

	Bool SpinLock::Get()
	{
#ifdef _SPIN_LOCK_USE_ASM_
		int a;
		while (1) {
			__asm__ __volatile__("xchg %%eax,%0":"=m"(_syo),"=a"(a):"a"(1));
			if (a == 0)
				break;
			usleep(14000);
		}
		return true;
#else
		return 0 == pthread_spin_lock((pthread_spinlock_t*)_syo);
#endif
	}

	Void SpinLock::Free()
	{
#ifdef _SPIN_LOCK_USE_ASM_
		__asm__ __volatile__("xchg %%eax,%0" : "=m"(_syo) : "a"(0));
//			: "%eax", "memory"
#else
		pthread_spin_unlock((pthread_spinlock_t*)_syo);
#endif
	}

	Bool SpinLock::TryGet()
	{
#ifdef _SPIN_LOCK_USE_ASM_
		int a;
		__asm__ __volatile__("xchg %%eax,%0":"=m"(_syo),"=a"(a):"a"(1));
		return a == 0;
#else
		return 0 == pthread_spin_trylock((pthread_spinlock_t*)_syo);
#endif
	}

}
}
