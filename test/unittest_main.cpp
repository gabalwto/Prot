#include "unittest.h"
#include "memops.hh"
#include <system/console.hh>
#include <system/debug.hh>

using namespace Prot;

Void ShowHelp()
{
	System::Console::PrintF("%s", "<This EXE> [Option Lists]\n"
		"  [Options]\n"
		"    -st        single thread test.\n"
		);
}

Bool handle_options(int argc, char* argv[])
{
	for (int i = 1; i < argc; i++)
	{
		if (0 == Memory::Comp("-st", argv[i], 4))
		{
			UnitTest::AddOption("Threads", "Single");
			continue;
		}
		if (0 == Memory::Comp("-h", argv[i], 3))
		{
			ShowHelp();
			return False;
		}
	}
	return True;
}

#include <stdio.h>
int main(int argc, char* argv[])
{
	int rv;
	UnitTest::InitTest();
	if (!handle_options(argc, argv))
		return -1;
	rv = UnitTest::RunTest();
	UnitTest::DoneTest();
	if (System::Debug::Attached())
		getchar();
	return rv;
}
