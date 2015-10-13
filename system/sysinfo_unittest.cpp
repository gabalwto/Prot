#include <test/ptutest.h>
#include "sysinfo.hh"


PTest(SystemInfo, BasicInfo)
{
	EXPECT_NE(0, System::Information::NumOfCPUCores());
}