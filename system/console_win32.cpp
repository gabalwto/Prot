#include <windows.h>
#include <stdio.h>

namespace Prot {
namespace System {

	const WORD _PCON_COLTB[] = {
	/* black        */  0,
	/* white        */  FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY,
	/* dark white   */  FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE,
	/* dark black   */  FOREGROUND_INTENSITY,
	/* light red    */  FOREGROUND_INTENSITY|FOREGROUND_RED,
	/* light green  */  FOREGROUND_INTENSITY|FOREGROUND_GREEN,
	/* light blue   */  FOREGROUND_INTENSITY|FOREGROUND_BLUE,
	/* light yellow */  FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN,
	/* light pink   */  FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE,
	/* light cyan   */  FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE,
	/* dark red     */  FOREGROUND_RED,
	/* dark green   */  FOREGROUND_GREEN,
	/* dark blue    */  FOREGROUND_BLUE,
	/* dark yellow  */  FOREGROUND_RED|FOREGROUND_GREEN,     // Purple
	/* dark pink    */  FOREGROUND_RED|FOREGROUND_BLUE,
	/* dark cyan    */  FOREGROUND_GREEN|FOREGROUND_BLUE,
	};

	static Console::Color _PCON_CURCOL[] = {
		Console::DarkWhite,
		Console::DarkWhite,
		Console::DarkWhite,
	};

	const DWORD _PCON_FHTAB[] = {
		STD_INPUT_HANDLE,
		STD_OUTPUT_HANDLE,
		STD_ERROR_HANDLE
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
			SetConsoleTextAttribute(GetStdHandle(_PCON_FHTAB[stdev]), _PCON_COLTB[color]);
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
		HANDLE hDev = GetStdHandle(_PCON_FHTAB[stdev]);
		va_list pv;
		va_start(pv, fmt);
		int retv = ::vfprintf(_PCON_FPTAB[stdev], fmt, pv);
		va_end(pv);
		return retv;
	}

	Int32 Console::CFPrintF(Color color, STDFILE stdev, const Char* fmt, ...)
	{
		HANDLE hDev = GetStdHandle(_PCON_FHTAB[stdev]);
		bool bSet = (color != _PCON_CURCOL[stdev]);
		if (bSet) SetConsoleTextAttribute(hDev, _PCON_COLTB[color]);
		va_list pv;
		va_start(pv, fmt);
		int retv = ::vfprintf(_PCON_FPTAB[stdev], fmt, pv);
		va_end(pv);
		if (bSet) SetConsoleTextAttribute(hDev, _PCON_COLTB[_PCON_CURCOL[stdev]]);
		return retv;
	}

}
}