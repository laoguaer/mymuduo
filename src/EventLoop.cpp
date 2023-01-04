#include <sys/fcntl.h>
#include<sys/eventfd.h>
#include <unistd.h>

#include "EventLoop.h"
#include "logger.h"
#include "Poller.h"
#include "Channel.h"

thread_local EventLoop *t_loolInThisThread = nullptr;

const int kPollTimeMs = 10000;

static int createEventfd() {
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0) {
		LOG_FATAL("eventfd error: %d", errno);
	}
	return evtfd;
}

EventLoop::EventLoop()
	: looping_(false)
	, quit_(false)
	, threadId_(CurrentThread::tid())
	, callingPendingFunctors_(false)
	, poller_(Poller::newDefaultPoller(this))
	, wakeupFd_(createEventfd())
	, wakeupChannel_(new Channel(this, wakeupFd_))
	, currentActiveChannel_(nullptr)
{
	LOG_DEBUG("EventLoop created %p in thread %d", this, threadId_);
	if (t_loolInThisThread) {
		LOG_FATAL("EventLoop exits in this Thread :%d", threadId_);
	}
	else {
		t_loolInThisThread = this;
	}

	wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
	wakeupChannel_->enableReading();
}
EventLoop::~EventLoop() {
	wakeupChannel_->disableAll();
	wakeupChannel_->remove();
	::close(wakeupFd_);
	t_loolInThisThread = nullptr;
}


void EventLoop::loop() {
	looping_ = true;
	quit_ = false;

	LOG_INFO("loop %p start looping", this);

	while (!quit_) {
		activeChannels_.clear();
		pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
		
		for (Channel *channel : activeChannels_) {
			channel->handleEvent(pollReturnTime_);
		}

		doPendingFuntors();
	}

	LOG_INFO("loop %p stop looping", this);
	looping_ = false;
}
void EventLoop::quit() {
	quit_ = true;
	if (!isInLoopThread()) {
		wakeup();
	}
}

void EventLoop::runInLoop(Functor cb) {
	if (isInLoopThread()) {
		cb();
	}
	else {
		queueInLoop(cb);
	}
}
void EventLoop::queueInLoop(Functor cb) {
	{
		std::lock_guard<std::mutex> lock(mutex_);
		pendingFunctors_.emplace_back(cb);
	}
	if (!isInLoopThread() || callingPendingFunctors_) {
		wakeup();
	}
}

void EventLoop::wakeup() {
	uint64_t one = 1;
	ssize_t n = write(wakeupFd_, &one, sizeof one);
	if (n != sizeof one) {
		LOG_ERROR("EventLoop::wakeup() write %ld bytes instead of 8", n);
	}
}

void EventLoop::updateChannel(Channel *channel) {
	poller_->updateChannel(channel);
}
void EventLoop::removeChannel(Channel *channel) {
	poller_->removeChannel(channel);
}
bool EventLoop::hasChannel(Channel *channel) {
	return poller_->hasChannel(channel);
}

void EventLoop::doPendingFuntors() {
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		pendingFunctors_.swap(functors);
	}

	for (const auto &functor : functors) {
		functor();
	}
	callingPendingFunctors_ = false;
}

// 单独给wakeupChannel用做回调
void EventLoop::handleRead() {
	uint64_t one = 1;
	ssize_t n = read(wakeupFd_, &one, sizeof one);
	if (n != sizeof one) {
		LOG_ERROR("handlerRead read %ld bytes instead of 8", n);
	}
}