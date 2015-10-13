#include "console.hh"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

namespace Prot {
namespace System {

	const char* _PCON_COLTB[] = {
	/* black        */  "\033[1m\033[30m",
	/* white        */  "\033[1m\033[37m",//FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY,
	/* dark white   */  "\033[37m",//FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE,
	/* dark black   */  "\033[30m",//FOREGROUND_INTENSITY,
	/* light red    */  "\033[1m\033[31m",//FOREGROUND_INTENSITY|FOREGROUND_RED,
	/* light green  */  "\033[1m\033[32m",//FOREGROUND_INTENSITY|FOREGROUND_GREEN,
	/* light blue   */  "\033[1m\033[34m",//FOREGROUND_INTENSITY|FOREGROUND_BLUE,
	/* light yellow */  "\033[1m\033[33m",//FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN,
	/* light pink   */  "\033[1m\033[35m",//FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE,
	/* light cyan   */  "\033[1m\033[36m",//FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE,
	/* dark red     */  "\033[31m",//FOREGROUND_RED,
	/* dark green   */  "\033[32m",//FOREGROUND_GREEN,
	/* dark blue    */  "\033[34m",//FOREGROUND_BLUE,
	/* dark yellow  */  "\033[33m",//FOREGROUND_RED|FOREGROUND_GREEN,     // Purple
	/* dark pink    */  "\033[35m",//FOREGROUND_RED|FOREGROUND_BLUE,
	/* dark cyan    */  "\033[36m",//FOREGROUND_GREEN|FOREGROUND_BLUE,
	};

	static Console::Color _PCON_CURCOL[] = {
		Console::DarkWhite,
		Console::DarkWhite,
		Console::DarkWhite,
	};

	static FILE* _PCON_FPTAB[] = {
		stdin,
		stdout,
		stderr
	};

	Console::Color Console::SetTextColor(Console::Color color, STDFILE stdev)
	{
		Color old = _PCON_CURCOL[stdev];
		if (color != old)
		{
			_PCON_CURCOL[stdev] = color;
		}
		return old;
	}

	Int32 Console::PrintF(const Char* fmt, ...)
	{
		va_list pv;
		va_start(pv, fmt);
		int retv = ::vfprintf(_PCON_FPTAB[StdOut], fmt, pv);
		va_end(pv);
		return retv;
	}

	Int32 Console::EPrintF(const Char* fmt, ...)
	{
		va_list pv;
		va_start(pv, fmt);
		int retv = ::vfprintf(_PCON_FPTAB[StdErr], fmt, pv);
		va_end(pv);
		return retv;
	}

	Int32 Console::FPrintF(STDFILE stdev, const Char* fmt, ...)
	{
		va_list pv;
		va_start(pv, fmt);
		int retv = ::vfprintf(_PCON_FPTAB[stdev], fmt, pv);
		va_end(pv);
		return retv;
	}

    static bool IsTermSupportColorOutput(FILE* stdfile)
    {
		if (isatty(fileno(stdfile)) == 0)
			return false;
		const char* tn = getenv("TERM");
		if (!tn)
			return false;
		return (0 == strcmp(tn, "xterm") ||
			0 == strcmp(tn, "xterm-color") ||
			0 == strcmp(tn, "xterm-256color") ||
			0 == strcmp(tn, "screen") ||
			0 == strcmp(tn, "screen-256color") ||
			0 == strcmp(tn, "linux") ||
			0 == strcmp(tn, "cygwin"));
	}

    static Int32 _PCON_STTCNT[] = {0,0,0};
    static bool _PCON_STTSCO[] = {false,false,false};

    inline bool IsStdTermSupportColorOutput(STDFILE stdev, FILE* fp)
	{
		Int32 n = _PCON_STTCNT[stdev]++;
		bool bs = _PCON_STTSCO[stdev];
		if (!(n & (n-1)))
		{
			if (bs != IsTermSupportColorOutput(fp))
			{
				bs = !bs;
				_PCON_STTCNT[stdev] = 0;
				_PCON_STTSCO[stdev] = bs;
			}
		}
		return bs;
	}

	Int32 Console::CFPrintF(Color color, STDFILE stdev, const Char* fmt, ...)
	{
		FILE* fcp = _PCON_FPTAB[stdev];
		bool bSet = (color != Console::DarkWhite && IsStdTermSupportColorOutput(stdev, fcp));
		if (bSet) fprintf(fcp, _PCON_COLTB[color]);
		va_list pv;
		va_start(pv, fmt);
		int retv = ::vfprintf(fcp, fmt, pv);
		va_end(pv);
		if (bSet) fprintf(fcp, "\033[0m");
		return retv;
	}

}
}
