#ifndef __PROT_LIBRARY_BASIC_ALLOCATE_H_
#define __PROT_LIBRARY_BASIC_ALLOCATE_H_

#include "defines.hh"
#include <stdlib.h>
#if __PT_DEBUG_
#include <system/debug.hh>
#endif

namespace Prot {
namespace Memory {

	inline static AnyPtr Alloc(Size nbytes)
	{
		return nbytes ? malloc(nbytes) : NULL; 
	}
	inline static AnyPtr Reloc(AnyPtr ptr, Size nbytes)
	{
		return realloc(ptr, nbytes);
	}
	/* re-allocate memory only copy nb_cpy bytes */
	inline static AnyPtr Reloc(AnyPtr ptr, Size nbytes, Size nb_cpy)
	{
		if (nb_cpy + 1000 < nbytes)
		{
			ptr = realloc(ptr, nb_cpy);
		}
		return realloc(ptr, nbytes);
	}
	inline static Void Free(AnyPtr ptr)
	{
#if __PT_DEBUG_
		if (ptr == Null)
			System::Debug::Break();
#endif
		if (ptr) free(ptr);
	}

#ifndef NOPTEMPLATE
	template <typename T>
	inline static Ptr<T> AllocT(Size ncount = 1)
	{
		return (T*)Alloc(ncount * sizeof(T)); //ncount ? malloc(ncount * (sizeof(T))) : NULL;
	}
	template <typename T>
	inline static Ptr<T> RelocT(Ptr<T> ptr, Size ncount)
	{
		return (T*)Reloc(ptr, ncount * sizeof(T));
	}
	template <typename T>
	inline static Ptr<T> RelocT(Ptr<T> ptr, Size ncount, Size nc_cpy)
	{
		return (T*)Reloc(ptr, ncount * sizeof(T), nc_cpy*sizeof(T));
	}
#endif

}
}

#endif
