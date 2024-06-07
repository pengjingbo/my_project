#ifndef _INETADRESS_H_
#define _INETADRESS_H_
#include <arpa/inet.h>
#include <string>

class InetAddress
{
public:
    explicit InetAddress(uint16_t port = 0, std::string IP = "127.0.0.1");
    explicit InetAddress(const sockaddr_in &addr) : addr_(addr) {}
    const std::string toIP() const;
    const uint16_t toPort() const;
    const std::string toIpPort() const;
    const sockaddr_in *getaddr() const{ return &addr_; }
    void setSockAddrInet(sockaddr_in &addr) { addr_ = addr; }

private:
    sockaddr_in addr_;
};

#endif