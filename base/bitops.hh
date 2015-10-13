#ifndef __PROT_LIBRARY_BASIC_BITOPS_H_
#define __PROT_LIBRARY_BASIC_BITOPS_H_

#include "defines.hh"

namespace Prot {
namespace Bits {
	template <typename T>
	inline T Or(T num1, T num2)
	{
		return num1 | num2;
	}
	template <typename T>
	inline Void Or(T* out, T* num1, T* num2)
	{
		Size* p1 = (Size*)num1;
		Size* p2 = (Size*)num2;
		Size* p = (Size*)out;
		Size n = sizeof(T);
		while (n >= sizeof(Size))
		{
			*p= *p1 | *p2;
			p++; p1++; p2++;
			n -= sizeof(Size);
		}
		while (n)
		{
			*(Byte*)p = *(Byte*)p1 | *(Byte*)p2;
			((Byte*)p)++;
			((Byte*)p1)++;
			((Byte*)p2)++;
			n--;
		}
	}

	template <typename T1, typename T2>
	/** Align to upper bound, bound = 4 result (0,1,2,3) => 0 */
	inline T1 AlignUp(T1 number, T2 bound)
	{
		return ((number - 1) | ((T1)bound - 1)) + 1;
	}
	template <typename T1, typename T2>
	/** Align to lower bound, bound = 4 result (0) => 0, (1,2,3,4) => 4 */
	inline T1 AlignDown(T1 number, T2 bound)
	{
		return number & ~(T1)bound;
	}
	template <typename T1, typename T2>
	/** Align to mask, bound = 4 result (0,1,2,3) => 3 */
	inline T1 Align_1(T1 number, T2 bound)
	{
		return number | ((T1)bound - 1);
	}
}
}

#endif
