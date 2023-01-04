#include <unistd.h>
#include <netinet/tcp.h>

#include "logger.h"
#include "Socket.h"
#include "InetAddress.h"

Socket::~Socket() {
	::close(fd_);
}

void Socket::bindAdress(const InetAddress &localaddr) {
	if (0 != bind(fd_, (sockaddr*)localaddr.getSockAddr(), sizeof (sockaddr_in))) {
		LOG_FATAL("bind %d fd fatal: %d", fd_, errno);
	}
}
void Socket::listen() {
	if (0 != ::listen(fd_, 1024)) {
		LOG_FATAL("listen %d fd fatal : %d", fd_, errno);
	}
}
int Socket::accept(InetAddress *peeraddr) {
	sockaddr_in addr;
	memset(&addr, 0, sizeof addr);
	socklen_t len = sizeof addr;
	int connfd = ::accept4(fd_, (sockaddr*)&addr, &len, SOCK_CLOEXEC | SOCK_NONBLOCK);
	if (connfd >= 0) {
		peeraddr->setSockAddr(addr);
	}
	return connfd;
}

void Socket::shutDownWrite() {
	if (::shutdown(fd_, SHUT_WR) == -1) {
		LOG_ERROR("shutDown error %d fd : %d", fd_, errno);
	}
}

void Socket::setTcpNoDelay(bool on) {
	int opt = on ? 1 : 0;
	if (::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof opt) == -1) {
		LOG_ERROR("");
	}
}
void Socket::setReuseAddr(bool on) {
	int opt = on ? 1 : 0;
	if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) == -1) {
		LOG_ERROR("");
	}
}
void Socket::setReusePort(bool on) {
	int opt = on ? 1 : 0;
	if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof opt)) {
		LOG_ERROR("");
	}
}
void Socket::setKeepAlive(bool on) {
	int opt = on ? 1 : 0;
	if (::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof opt) == -1)  {
		LOG_ERROR("");
	}
}