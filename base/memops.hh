#ifndef __PROT_LIBRARY_BASIC_MEMOPS_H_
#define __PROT_LIBRARY_BASIC_MEMOPS_H_

#include "defines.hh"
#include <memory.h>

namespace Prot {
namespace Memory {
	inline static Void Fill(AnyPtr ptr, Size nbytes, Byte chr)
	{
		memset(ptr, chr, nbytes);
	}
	inline static Void Copy(AnyPtr dest, AnyPtr from, Size nbytes)
	{
		memcpy(dest, from, nbytes);
	}
	inline static Long Comp(AnyPtr ptr1, AnyPtr ptr2, Size nbytes)
	{
		return memcmp(ptr1, ptr2, nbytes);
	}
	inline static AnyPtr Find(AnyPtr ptr, Size nbytes, Byte chr)
	{
		return (AnyPtr)memchr((const void*)ptr, chr, nbytes);
	}

#ifndef NOPTEMPLATE
	template <typename T>
	/** Use Memory::Fill<T>() access on Ptr<T> */
	inline static Void FillT(Ptr<T> ptr, Size nelems, const T& ele)
	{
		if (sizeof(T) == 1) {
			memset(ptr, ele, nelems);
		}
		else 
		for (Size I = 0; I < nelems; I++)
		{
			ptr[I] = ele;
		}
	}
	template <typename T>
	/** Use Memory::Copy<T>() access on Ptr<T> */
	inline static Void CopyT(Ptr<T> dest, Ptr<T> src, Size nelems)
	{
		memcpy(dest, src, nelems*sizeof(T));
	}
	template <typename T>
	/** Use Memory::Comp<T>() access on Ptr<T> */
	inline static Long CompT(Ptr<T> ptr1, Ptr<T> ptr2, Size nelems)
	{
		return memcmp(ptr1, ptr2, nelems*sizeof(T));
	}
	template <typename T>
	/** Use Memory::Find<T>() access on Ptr<T> */
	inline static Ptr<T> FindT(Ptr<T> ptr, Size nelems, const T& ele)
	{
		if (sizeof(T) == 1)
			return (T*)memchr((const void*)ptr, ele, nelems);
		for (Size I = 0; I < nelems; I++)
		{
			if (0 == memcmp(&ptr[I], &ele, sizeof(T)))
				return &ptr[I];
		}
		return NullPtr;
	}
	//template <typename T>
	//inline static Ptr<T> Find(Ptr<T> ptr, Size nelems, const T ele)
	//{
	//	if (sizeof(T) == 1)
	//		return memchr((const void*)ptr, ele, nelems);
	//	for (Size I = 0; I < nelems; I++)
	//	{
	//		if (0 == memcmp(ptr[I], &ele, sizeof(T)))
	//			return &ptr[I];
	//	}
	//	return Null;
	//}
#endif
}
}

#endif
