#ifndef __PROT_LIBRARY_SYSTEM_CONSOLE_H_
#define __PROT_LIBRARY_SYSTEM_CONSOLE_H_

#include "defines.hh"
    
namespace Prot {
namespace System {

	typedef enum {
		StdIn,
		StdOut,
		StdErr
	} STDFILE;

	class Console
	{
	public:
		typedef enum {
			Black,
			White,
			DarkWhite,
			DefaultText = DarkWhite,
			DarkBlack,
			Gray = DarkBlack,
			Red,
			Green,
			Blue,
			Yellow,
			Pink,
			Cyan,       // Aoi
			DarkRed,
			DarkGreen,
			DarkBlue,
			DarkYellow,
			DarkPink,
			Purple = DarkPink,     // DarkPink
			DarkCyan
		} Color;

		//static void Redirect(File* link_stdout, File* link_stderr);
		//static void Write(String& text);

		/* return previous console text color */
		static Color SetTextColor(Color color, STDFILE stdev);
		static Int32 PrintF(const Char* fmt, ...);
		static Int32 EPrintF(const Char* fmt, ...);
		static Int32 FPrintF(STDFILE stdev, const Char* fmt, ...);
		static Int32 CFPrintF(Color color, STDFILE stdev, const Char* fmt, ...);

		//static void Read(String& text);
		//static Bool ReadLine();
	};

}
}

#endif
