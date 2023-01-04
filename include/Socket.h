#pragma once 
#include "noncopyable.h"

class InetAddress;


class Socket : noncopyable {
public:
	explicit Socket(int fd) : fd_(fd) {}

	~Socket();

	int fd() const { return fd_; }
	void bindAdress(const InetAddress &localaddr);
	void listen();
	int accept(InetAddress *peeraddr);
	void shutDownWrite();

	void setTcpNoDelay(bool on);
	void setReuseAddr(bool on);
	void setReusePort(bool on);
	void setKeepAlive(bool on);


private:
	const int fd_;
};