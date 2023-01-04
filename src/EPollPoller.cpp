#include <unistd.h>
#include <string.h>

#include "EPollPoller.h"
#include "logger.h"
#include "Channel.h"
#include "CurrentThread.h"

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop *loop) 
	: Poller(loop)
	, epollfd_(::epoll_create1(EPOLL_CLOEXEC))
	, events_(kInitEventListSize) 
{
	if (epollfd_ < 0) {
		LOG_FATAL("epoll_create1 error:%d", errno);
	}
}
EPollPoller::~EPollPoller() {
	::close(epollfd_);
}

Timestamp EPollPoller::poll(int timeoutms, ChannelList *activeChannels) {

	int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutms);
	int saveErrno = errno;

	if (numEvents > 0) {
		LOG_INFO("%d events happened", numEvents);
		fillActiveChannels(numEvents, activeChannels);
		if (static_cast<int>(channels_.size()) == numEvents) {
			events_.resize(2 * events_.size());
		}
	}
	else if (numEvents == 0) {
		LOG_INFO("poll fd cnt: %ld thread: %d", channels_.size(), CurrentThread::tid());
	}
	else {
		if (saveErrno != EINTR) {
			errno = saveErrno;
			LOG_ERROR("EpollPoller::poll() Err");
		}
	}

	return Timestamp::now();
}

void EPollPoller::updateChannel(Channel* channel) {
	const int index = channel->index();
	LOG_INFO("fd = %d events = %d index = %d", channel->fd(), channel->events(), index);

	if (index == kNew || index == kDeleted) {
		if (index == kNew) {
			int fd = channel->fd();
			channels_[fd] = channel;
		}

		channel->set_index(kAdded);
		update(EPOLL_CTL_ADD, channel);
	} 
	else {
		int fd = channel->fd();
		if (channel->isNoneEvent()) {
			update(EPOLL_CTL_DEL, channel);
			channel->set_index(kDeleted);
		}
		else {
			update(EPOLL_CTL_MOD, channel);
		}
	}
}
void EPollPoller::removeChannel(Channel* channel) {
	int fd = channel->fd();
	channels_.erase(fd);

	LOG_INFO(" success");

	int index = channel->index();
	if (index == kAdded) {
		update(EPOLL_CTL_DEL, channel);
	}
	channel->set_index(kNew);
}



void EPollPoller::update(int operation, Channel *channel) {
	epoll_event event;
	bzero(&event, sizeof event);
	
	int fd = channel->fd();
	
	event.events = channel->events();
	event.data.ptr = channel;

	if (::epoll_ctl(epollfd_, operation, fd, &event) == -1) {
		if (operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll_ctl del error:%d\n", errno);
        }
        else
        {
            LOG_FATAL("epoll_ctl add/mod error:%d\n", errno);
        }
	}
}

void EPollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const {
	for (int i = 0; i < numEvents; i++) {
		Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
		LOG_INFO("channel->%d", channel->fd());
		channel->set_revents(events_[i].events);
		activeChannels->push_back(channel);
	}
}
