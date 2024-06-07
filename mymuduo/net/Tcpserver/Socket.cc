#include "Socket.h"
#include "InetAddress.h"
#include "Logger.h"

#include <unistd.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>

Socket::~Socket()
{
    ::close(sockfd_);
}

// 绑定服务端本地的ip地址结构
void Socket::bindAddress(const InetAddress &localaddr)
{
    if (::bind(sockfd_,(sockaddr*)localaddr.getaddr(), sizeof(sockaddr_in)) != 0)
    {
        LOG_ERROR("bind sockfd:%d failed errno:%d\n", sockfd_,errno);
    }
}
//设置sockfd_的监听上限
void Socket::listen()
{
    if (::listen(sockfd_, 1024) != 0)
    {
        LOG_ERROR("listen sockfd:%d failed\n", sockfd_);
    }
}

int Socket::accept(InetAddress &peeraddr)
{
    sockaddr_in addr;
    socklen_t len = sizeof addr;
    bzero(&addr, 0);
    int connfd = ::accept4(sockfd_, (sockaddr *)&addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd >= 0)
    {
        peeraddr.setSockAddrInet(addr);
    }
    return connfd;
}

// 关闭tcp的写端，即不能向对端发送数据了，如果这个时候没有
void Socket::shutdownWrite()
{
    //关闭sockfd的写端，触发sockfd的EPOLLHUP事件
    if (::shutdown(sockfd_, SHUT_WR) != 0)
    {
        LOG_WARN("shutdownWrite error\n");
    }
}

//设置是否禁用Nagle算法
void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, (socklen_t)(sizeof optval));
}

//SO_REUSEADDR是让端口释放后立即就可以被再次使用。
void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, (socklen_t)(sizeof optval));
}
//setReusePort使得多进程或者多线程创建多个绑定同一个ip:port的监听socket
void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, (socklen_t)(sizeof optval));
}
//setKeepAlive设置TCP套接字的选项，确保连接的可靠性和稳定性
void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, (socklen_t)(sizeof optval));
}
