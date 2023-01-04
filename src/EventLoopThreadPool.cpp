#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop, const std::string &name)
	: baseloop_(baseloop)
	, name_(name)
	, started_(false)
	, numThreads_(0)
	, next_(0) {}

EventLoopThreadPool::~EventLoopThreadPool() {}

void EventLoopThreadPool::start(const ThreadInitCallback &threadInitCallback) {
	started_ = true;
	for (int i = 0; i < numThreads_; i++) {
		char buf[name_.size() + 32] = {0};
		snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);

		// new EventLoopThread-》（ThreadInintFunc name）

		EventLoopThread *t = new EventLoopThread(threadInitCallback, buf);
		threads_.push_back(std::unique_ptr<EventLoopThread>(t));
		loops_.push_back(t->startLoop());
	}

	if (numThreads_ == 0 && threadInitCallback) {
		threadInitCallback(baseloop_);
	}
}

EventLoop *EventLoopThreadPool::getNextLoop() {
	EventLoop *loop = baseloop_;
	
	if (!loops_.empty()) {
		loop = loops_[next_++];
		next_ %= loops_.size();
	}

	return loop;
}

std::vector<EventLoop *> EventLoopThreadPool::getAllLoops() {
	if (loops_.empty()) {
		return std::vector<EventLoop*>(1, baseloop_);
	}
	else {
		return loops_;
	}
}
