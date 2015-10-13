#ifndef __PROT_LIBRARY_SYSTEM_LOCK_H_
#define __PROT_LIBRARY_SYSTEM_LOCK_H_

#include "defines.hh"

namespace Prot {
namespace Lock {

	enum LockType {
		TypeInvalid = 0,
		TypeSpinning = 1,	// spin lock
		TypeRecursive = 2,	// recursive mutex lock
		TypeSinglenter = 3,	// mutex lock
		TypeSemaphore = 4,
		TypeAutoResetEvent = 5,
		TypeManualResetEvent = 6,
		TypeConditionVariable = 7
	};

	class ILock
	{
	public:
		/** If lock initial failed, type got invalid */
		virtual LockType Type() = 0;
		virtual Bool Get() = 0;
		virtual Void Free() = 0;
		virtual Bool TryGet() = 0;
	};

	class NoLock :public ILock
	{
	public:
		virtual LockType Type() { return  TypeInvalid; }
		virtual Bool Get() { return true; }
		virtual Void Free(){}
		virtual Bool TryGet() { return true; }
	};

	class SpinLock :public ILock
	{
	public:
		SpinLock();
#if __PLATFORM_OSYS_ != PTOS_WINDOWS
		~SpinLock();
#endif
		virtual LockType Type();
		virtual Bool Get();
		virtual Void Free();
		virtual Bool TryGet();
	private:
		AnyPtr _syo;
	};

	class SinglenterLock :public ILock
	{
	public:
		SinglenterLock();
		~SinglenterLock();
		virtual LockType Type();
		virtual Bool Get();
		virtual Void Free();
		virtual Bool TryGet();
	private:
		AnyPtr _syo;
	};

	class RecursiveLock :public ILock
	{
	public:
		RecursiveLock();
		~RecursiveLock();
		virtual LockType Type();
		virtual Bool Get();
		virtual Void Free();
		virtual Bool TryGet();
	private:
		AnyPtr _syo;
	};

	typedef class SinglenterLock Mutex;
	typedef class RecursiveLock RecursiveMutex;

}
}

#endif
