#include "Acceptor.h"
#include "logger.h"
#include "InetAddress.h"

#include <arpa/inet.h>
#include <unistd.h>

static int createNonblocking() {
	int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, IPPROTO_TCP);

	if (sockfd < 0) {
		LOG_FATAL("%s:%s:%d create socket fatal :%d", __FILE__, __FUNCTION__, __LINE__, errno);
	}
	LOG_INFO("sockfd created");
	return sockfd;
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport)
	: loop_(loop)
	, acceptSocket_(createNonblocking())
	, acceptChannel_(loop, acceptSocket_.fd())
	, listenning_(false) 
{
	acceptSocket_.setReuseAddr(true);
	acceptSocket_.setReusePort(reuseport);
	acceptSocket_.bindAdress(listenAddr);
	acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}
Acceptor::~Acceptor() {
	acceptChannel_.disableAll();
	acceptChannel_.remove();
}
void Acceptor::listen() {
	listenning_ = true;
	acceptSocket_.listen();
	acceptChannel_.enableReading();
}

void Acceptor::handleRead() {
	InetAddress peerAddr;
	int connfd = acceptSocket_.accept(&peerAddr);
	if (connfd >= 0) {
		if (newConnectionCallback_) {
			newConnectionCallback_(connfd, peerAddr);
		}
		else {
			::close(connfd);
		}
	}
	else {
		if (errno == EMFILE) {
			LOG_ERROR("Acceptor : file descriptor reched max");
		}
	}
}