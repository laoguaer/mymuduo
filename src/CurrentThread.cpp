#include "CurrentThread.h"

namespace CurrentThread {

thread_local int t_cachedTid = 0;

void CacheTid() {

	if (t_cachedTid == 0) {
		t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
	}
}

}