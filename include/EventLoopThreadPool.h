#include "EventLoopThread.h"

class EventLoopThreadPool : noncopyable {
public:
	using ThreadInitCallback = std::function<void(EventLoop*)>;

	EventLoopThreadPool(EventLoop *baseloop, const std::string &name);
	~EventLoopThreadPool();

	void setThreadNum(int num) { numThreads_ = num; }

	void start(const ThreadInitCallback &cb = ThreadInitCallback());

	EventLoop* getNextLoop();

	std::vector<EventLoop*> getAllLoops();

	bool started() const { return started_; }

	const std::string name() const { return name_; }
private:
	EventLoop *baseloop_;
	std::string name_;
	bool started_;
	int numThreads_;
	int next_;
	std::vector<std::unique_ptr<EventLoopThread>> threads_;
	std::vector<EventLoop*> loops_;
};