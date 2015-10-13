#include "lock.hh"

#if __PLATFORM_OSYS_ == PTOS_WINDOWS
#include "lock_win32.cpp"
#elif __PLATFORM_OSYS_ == PTOS_LINUX
#include "lock_posix.cpp"
#else
#error "no implement os !"
#endif
