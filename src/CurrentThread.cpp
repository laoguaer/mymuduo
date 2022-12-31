#include "CurrentThread.h"

namespace CurrentThread {

void CacheTid() {
	if (t_cachedTid == 0) {
		t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
	}
}

}