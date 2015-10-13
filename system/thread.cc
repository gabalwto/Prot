#include "thread.hh"

#if __PLATFORM_OSYS_ == PTOS_WINDOWS
#include "thread_win32.cpp"
#elif __PLATFORM_OSYS_ == PTOS_LINUX
#include "thread_posix.cpp"
#else
#error "no implement os !"
#endif
