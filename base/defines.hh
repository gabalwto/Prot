#ifndef __PROT_LIBRARY_BASIC_DEFINES_H_
#define __PROT_LIBRARY_BASIC_DEFINES_H_

namespace Prot {

#if defined(_DEBUG) || defined(__PROT_BUILD_DEBUG_)
#define __PT_DEBUG_ 1
#else
#define __PT_DEBUG_ 0
#endif

#define PTOS_UNKNOWN 0
#define PTOS_WINDOWS 1
#define PTOS_LINUX 2

#define PTCL_UNKNOWN 0
#define PTCL_MSVC 1
#define PTCL_GNUC 2

#if defined(_MSC_VER)
#define __PLATFORM_CL_ PTCL_MSVC
#elif defined(__GNUC__)
#define __PLATFORM_CL_ PTCL_GNUC
#endif

#if defined(_WIN64)
#define __PLATFORM_OSYS_ PTOS_WINDOWS
#define __PLATFORM_BITS_ 64
#elif defined(_WIN32)
#define __PLATFORM_OSYS_ PTOS_WINDOWS
#define __PLATFORM_BITS_ 32
#elif defined(__linux__)
#define __PLATFORM_OSYS_ PTOS_LINUX
#if defined(__i386__)
#define __PLATFORM_BITS_ 32
#elif defined(__x86_64__)
#define __PLATFORM_BITS_ 64
#endif
#else
#error "Unknown Platform"
#define __PLATFORM_OSYS_ PTOS_UNKNOWN
#endif


	typedef bool Bool;
	#define True true
	#define False false

	typedef char Int8;
	typedef short Int16;
	typedef int Int32;
	typedef long long Int64;

	typedef unsigned char UInt8;
	typedef unsigned short UInt16;
	typedef unsigned int UInt32;
	typedef unsigned long long UInt64;

#if (__PLATFORM_BITS_ == 64)
	typedef Int64 Long;
	typedef UInt64 ULong;
#else
	typedef Int32 Long;
	typedef UInt32 ULong;
#endif

	typedef void Void;
	typedef double Float;

	typedef Int8 Char;
	typedef Int16 WChar;
	typedef UInt8 Byte;
	typedef ULong Size;

#define Null 0
#ifdef NOPTEMPLATE
	typedef void* AnyPtr;
#else
	enum NullPtr { NullPtr = 0 };

	template <typename T>
	class _PtPtr
	{
	public:
		inline _PtPtr(){ ptr = 0; }
		inline _PtPtr(T* p){ ptr = p; }
		inline _PtPtr(const T* p){ ptr = (T*)p; }
		inline _PtPtr(enum NullPtr n){ ptr = 0; }
		inline _PtPtr(const _PtPtr<T>& p){ ptr = p.ptr; }
		inline T* operator->() { return ptr; }
		inline const T* operator->() const { return ptr; }
		/** Compare with Potinter */
		template <typename OTHER_>
		inline Bool operator==(const OTHER_* p) const { return (Void*)p == (Void*)ptr; }
		template <typename OTHER_>
		inline Bool operator==(OTHER_* p) const { return (Void*)p == (Void*)ptr; }
		template <typename OTHER_>
		inline Bool operator!=(const OTHER_* p) const { return (Void*)p != (Void*)ptr; }
		template <typename OTHER_>
		inline Bool operator!=(OTHER_* p) const { return (Void*)p != (Void*)ptr; }
		/** Compare Pointer with _PtPtr<T> */
		template <typename OTHER_>
		inline friend Bool operator==(const OTHER_* p, const _PtPtr<T>& t) { return (Void*)p == (Void*)t.ptr; }
		template <typename OTHER_>
		inline friend Bool operator!=(const OTHER_* p, const _PtPtr<T>& t) { return (Void*)p != (Void*)t.ptr; }
		template <typename OTHER_>
		inline friend Bool operator==(const OTHER_* p, _PtPtr<T>& t) { return (Void*)p == (Void*)t.ptr; }
		template <typename OTHER_>
		inline friend Bool operator!=(const OTHER_* p, _PtPtr<T>& t) { return (Void*)p != (Void*)t.ptr; }
		/** Compare with _PtPtr<T> */
		template <typename OTHER_>
		inline Bool operator==(const _PtPtr<OTHER_>& p) const { return (Void*)p.ptr == (Void*)ptr; }
		template <typename OTHER_>
		inline Bool operator!=(const _PtPtr<OTHER_>& p) const { return (Void*)p.ptr != (Void*)ptr; }
		/** Compare with NullPtr */
		inline Bool operator==(enum NullPtr n) const { return (Void*)n == (Void*)ptr; }
		inline Bool operator!=(enum NullPtr n) const { return (Void*)n != (Void*)ptr; }
		/** Compare NullPtr with _PtPtr<T> */
		inline friend Bool operator==(enum NullPtr n, const _PtPtr<T>& p) { return (Void*)n == (Void*)p.ptr; }
		inline friend Bool operator!=(enum NullPtr n, const _PtPtr<T>& p) { return (Void*)n != (Void*)p.ptr; }
		template <typename OTHER_>
		inline Bool operator>(const _PtPtr<OTHER_>& p) const { return (Void*)ptr > (Void*)p.ptr; }
		template <typename OTHER_>
		inline Bool operator<(const _PtPtr<OTHER_>& p) const { return (Void*)ptr < (Void*)p.ptr; }
		template <typename OTHER_>
		inline Bool operator>=(const _PtPtr<OTHER_>& p) const { return (Void*)ptr >= (Void*)p.ptr; }
		template <typename OTHER_>
		inline Bool operator<=(const _PtPtr<OTHER_>& p) const { return (Void*)ptr <= (Void*)p.ptr; }
		/** Convert to AnyPtr */
		inline operator _PtPtr<Void>() { return (_PtPtr<Void>&)(ptr); }
		inline operator Void*() { return (Void*)ptr; }
		inline operator bool() { return ptr != 0; }
		template <typename OTHER_>
		inline _PtPtr<OTHER_>& AsPtr() const { return (_PtPtr<OTHER_>&)(ptr); }
		inline Bool IsNull() const { return 0 == ptr; }
		inline const Void* Address() { return ptr; }
	protected:
		T* ptr;
	};

	template <typename T>
	class Ptr :public _PtPtr<T>
	{
	public:
		inline Ptr():_PtPtr<T>(){}
		inline Ptr(T* p):_PtPtr<T>(p){}
		inline Ptr(const T* p):_PtPtr<T>(p){}
		inline Ptr(enum NullPtr n):_PtPtr<T>(n){}
		inline Ptr(const _PtPtr<T>& p):_PtPtr<T>(p){}
		/** constructor from anyptr void* */
		inline Ptr(const Void* p):_PtPtr<T>((T*)p){}
		/** potinter operation */
		inline T& operator*() { return *_PtPtr<T>::ptr; }
		inline const T& operator*() const { return *_PtPtr<T>::ptr; }
		inline T& operator[](Long i) { return _PtPtr<T>::ptr[i]; }
		inline const T& operator[](Long i) const { return _PtPtr<T>::ptr[i]; }
		template <typename OTHER_>
		inline Ptr<OTHER_>& AsPtr() const { return (Ptr<OTHER_>&)(_PtPtr<T>::ptr); }
	};

	template <>
	class Ptr<Void> :public _PtPtr<Void>
	{
	public:
		inline Ptr() :_PtPtr<Void>(){}
		template <typename T>
		inline Ptr(const T* p) : _PtPtr<Void>((Void*)p){}
		template <typename T>
		inline Ptr(const _PtPtr<T>& p) : _PtPtr<Void>(p.AsPtr<Void>()){}
		inline Ptr(enum NullPtr n) : _PtPtr<Void>(n){}
		template <typename T>
		inline operator T*() { return (T*)ptr; }
	};

	typedef Ptr<Void> AnyPtr;

#endif

}

#endif
