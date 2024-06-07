#include "InetAddress.h"

#include <strings.h>
#include <string.h>

InetAddress::InetAddress(uint16_t port, std::string IP)
{
    bzero(&addr_,sizeof(addr_));
    addr_.sin_family=AF_INET;
    addr_.sin_port=htons(port);
    addr_.sin_addr.s_addr=inet_addr(IP.c_str());
}
const std::string InetAddress::toIP()const
{
    char buf[64]={0};
    inet_ntop(AF_INET,&addr_.sin_addr,buf,64);
    return buf;
}
const uint16_t InetAddress::toPort()const
{
    return ntohs(addr_.sin_port);
}
const std::string InetAddress::toIpPort()const 
{
    char buf[64]={0};
    inet_ntop(AF_INET,&addr_.sin_addr,buf,64);
    std::string port_=" : ";
    port_.append(std::to_string(toPort()));
    strcat(buf,port_.c_str());
    return buf;
}
/* 
 #include<iostream>
int main()
{
    InetAddress addr(7000);
    std::cout<<addr.toIpPort()<<std::endl;
    auto addr2=addr.getaddr();
    std::cout<<ntohs(addr2->sin_port)<<std::endl;
    return 0;
}  */