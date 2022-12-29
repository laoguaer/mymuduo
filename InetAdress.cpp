#include "InetAdress.h"

InetAdress::InetAdress(uint16_t port, std::string ip) {
	memset(&addr_, 0, sizeof addr_);
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

std::string InetAdress::toIp() const {
	char buf[32] = {0};
	inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof buf);
	return buf;
}
uint16_t InetAdress::toPort() const {
	return ntohs(addr_.sin_port);
}
std::string InetAdress::toIpPort() const {
	char buf[32] = {0};
	inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof buf);
	int len = strlen(buf);
	sprintf(buf + len, ":%u", ntohs(addr_.sin_port));
	return buf;
}
