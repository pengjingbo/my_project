#ifndef _SOCKET_H
#define _SOCKET_H

class InetAddress;

// 对系统的socketfd进行一个类的一个封装
class Socket
{

public:
    explicit Socket(int sockfd) : sockfd_(sockfd) {}
    ~Socket();

    int fd() const { return sockfd_; }
    //绑定服务端本地的ip地址结构
    void bindAddress(const InetAddress &localaddr);
    void listen();

    int accept(InetAddress &peeraddr);

    //关闭tcp的写端，即服务器端不能通过sockfd_向客户端发送数据
    void shutdownWrite();
    //设置tcp连接时的一些选项
    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    int sockfd_;
};

#endif