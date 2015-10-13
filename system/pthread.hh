#ifndef __PROT_LIBRARY_SYSTEM_POOLTHREAD_H_
#define __PROT_LIBRARY_SYSTEM_POOLTHREAD_H_

#include "thread.hh"

namespace Prot {
namespace System {

	class PoolThread:public Thread
	{
	public:
		PoolThread(THREADSTARTINGENTRY func);
		~PoolThread();
		///* id == 0 means thread not created */
		//THREADID GetID();
		/* startup thread and return thread id,
			if thread still in thread func exec return 0,
			if thread create failed, return 0 */
		THREADID Run(Void* args);
		/* wait till thread exited or killed or
			thread in holding state or mesc milli-seconds ago
			if thread still running after waiting, return false
			otherwise, return true, even if thread not created */
		Bool Wait(Time::TimeUnit t);
		Bool Kill();
		typedef enum {
			Offline = 0,
			Running = 1,
			Holding = 2   // thread is ready now
		} ThreadState;
		ThreadState Status();
	private:
		AnyPtr _ptr;
	};

}
}

#endif
