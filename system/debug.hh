#ifndef __PROT_LIBRARY_SYSTEM_DEBUG_H_
#define __PROT_LIBRARY_SYSTEM_DEBUG_H_

#include "defines.hh"
#if (__PLATFORM_OSYS_ == PTOS_WINDOWS)
#include <windows.h>
#endif

namespace Prot {
namespace System {

	class Debug
	{
	public:
		inline static void Break()
		{
#if (__PLATFORM_OSYS_ == PTOS_WINDOWS)
#if (__PLATFORM_BITS_ == 32)
			__asm int 3;
#else
			DebugBreak();
#endif
#else
			((void(*)(void))(void*)"\xCC\xCC")();
#endif
		}

		static Bool Attached()
		{
#if (__PLATFORM_OSYS_ == PTOS_WINDOWS)
			return IsDebuggerPresent();
#else
			return False;
#endif
		}
	private:
	};
}
}

#endif
