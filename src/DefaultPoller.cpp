#include "Poller.h"
// Todo

#include  <stdlib.h>

Poller* Poller::newDefaultPoller(EventLoop *loop) {
	if (::getenv("MUDUO_USE_POLL")) {
		return nullptr;
	}
	else {
		return nullptr;
	}
}