#include "console.hh"

#if __PLATFORM_OSYS_ == PTOS_WINDOWS
#include "console_win32.cpp"
#elif __PLATFORM_OSYS_ == PTOS_LINUX
#include "console_linux.cpp"
#else
#error "no implement os !"
#endif
