#include <unistd.h>
#include <string.h>

#include "EPoller.h"
#include "logger.h"
#include "Channel.h"

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EPoller::EPoller(EventLoop *loop) 
	: Poller(loop)
	, epollfd_(epoll_create1(EPOLL_CLOEXEC))
	, events_(kInitEventListSize) 
{
	if (epollfd_ < 0) {
		LOG_FATAL("epoll_create1 error:%d", errno);
	}
}
EPoller::~EPoller() {
	close(epollfd_);
}

void EPoller::updateChannel(Channel* channel) {
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
void EPoller::removeChannel(Channel* channel) {
	int fd = channel->fd();
	channels_.erase(fd);

	int index = channel->index();
	if (index == kAdded) {
		update(EPOLL_CTL_DEL, channel);
	}
	channel->set_index(kNew);
}

Timestamp EPoller::poll(int timeoutms, ChannelList *activeChannels) {
	LOG_INFO("poll fd total count : %ld", channels_.size());

	int numEvents = epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutms);
	int saveErrno = errno;

	if (numEvents > 0) {
		LOG_INFO("%d events happened", numEvents);
		fillActiveChannels(numEvents, activeChannels);
		if (static_cast<int>(channels_.size()) == numEvents) {
			events_.resize(2 * events_.size());
		}
	}
	else if (numEvents == 0) {
		LOG_INFO("%s timeout", __FUNCTION__);
	}
	else {
		if (saveErrno != EINTR) {
			errno = saveErrno;
			LOG_ERROR("EpollPoller::poll() Err");
		}
	}

	return Timestamp::now();
}


void EPoller::update(int operation, Channel *channel) {
	epoll_event event;
	memset(&event, 0, sizeof event);
	event.events = channel->events();
	event.data.ptr = channel;
	event.data.fd = channel->fd();

	int fd = channel->fd();
	if (epoll_ctl(epollfd_, operation, fd, &event) == -1) {
		LOG_ERROR("epoll_ctl del error : %d", errno);
	}
	else {
		LOG_FATAL("epoll_ctl add/mod error : %d", errno);
	}
}

void EPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const {
	for (int i = 0; i < numEvents; i++) {
		Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
		channel->set_revents(events_[i].events);
		activeChannels->push_back(channel);
	}
}
