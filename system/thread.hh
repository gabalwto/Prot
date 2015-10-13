#ifndef __PROT_LIBRARY_SYSTEM_THREAD_H_
#define __PROT_LIBRARY_SYSTEM_THREAD_H_

#include "defines.hh"
#include <system/time/timeunit.hh>

namespace Prot {
namespace System {

	typedef Void (*THREADSTARTINGENTRY)(Void* args);
	typedef Void* THREADID;

	class Thread
	{
	public:
		Thread(THREADSTARTINGENTRY func);
		~Thread();
		/* id == 0 means thread not created */
		THREADID GetID();
		/* startup thread and return thread id,
			if thread already created, return 0,
			if thread create failed, return 0 */
		THREADID Run(Void* args);
		/* wait till thread exited or killed or mesc milli-seconds ago
			if thread still exist after waiting, return false
			otherwise, return true, even if thread not created */
		Bool Wait(Time::TimeUnit t);
		Bool Kill();
		typedef enum {
			Offline = 0,
			Running = 1
		} ThreadState;
		ThreadState Status();
	private:
		AnyPtr _thd;
	};

	class ThisThread
	{
	public:
		static THREADID GetID();
		/** if t is Empty, yield cpu time of current thread,
		    if t is Negative, the function is undefined.  **/
		static Void Sleep(Time::TimeUnit t);
		/** if msec == 0, yield cpu time of current thread,
			if msec < 0, the function is undefined.    **/
		static Void Sleep(Int32 msec);
	};
}
}

#endif
