#include "EventLoopThread.h"
#include "EventLoop.h"


EventLoopThread::EventLoopThread(const ThreadInitCallback &cb, const std::string &name) 
	: loop_(nullptr)
	, exiting_(false)
	, thread_(std::bind(&EventLoopThread::threadFunc, this), name)
	, mutex_()
	, cond_()
	, callback_(cb)
{
}
EventLoopThread::~EventLoopThread() {
	exiting_ = true;
	if (loop_ != nullptr) {
		loop_->quit();
		thread_.join();
	}
}

EventLoop* EventLoopThread::startLoop() {
	thread_.start();
	EventLoop *loop = nullptr;
	{
		std::unique_lock<std::mutex> lock(mutex_);
		// 需要threadfunc创建完loop才能继续
		while (loop_ == nullptr) {
			cond_.wait(lock);
		}
		loop = loop_;
	}
	return loop;
}

// Thread 执行的func
// 创建 loop 并执行 TcpServer设置的ThreadInit函数
void EventLoopThread::threadFunc() {
	EventLoop loop;

	if (callback_) {
		callback_(&loop);
	}
	{
		std::unique_lock<std::mutex> lock(mutex_);
		loop_ = &loop;
		cond_.notify_one();
	}
	loop.loop();
    std::unique_lock<std::mutex> lock(mutex_);
	loop_ = nullptr;
}