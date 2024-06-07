#include "Acceptor.h"
#include "InetAddress.h"
#include "Logger.h"

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// 定义并初始化服务端绑定的接收客户端连接的sockfd
static int createNonblocking()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0)
    {
        LOG_ERROR("%s:%s:%d listen socket create error:%d \n", __FILE__, __FUNCTION__, __LINE__, errno);
    }
    return sockfd;
}
Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport)
    : loop_(loop)
    , acceptSocket_(createNonblocking())
    , acceptChannel_(loop,acceptSocket_.fd())
    , listenning_(false)
{
    //设置acceptSocket_的一些连接属性
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(reuseport);
    acceptSocket_.bindAddress(listenAddr);
    //当有新的连接产生时 => acceptfd上产生读事件，并执行读事件的回调 
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead,this));
}

Acceptor::~Acceptor()
{
    // 将acceptFd的全部注册事件关闭(其实也就只有读事件)
    acceptChannel_.disableAll();
    // 将acceptFd从baseloop_的poller中移除
    acceptChannel_.remove();
}

void Acceptor::listen()
{
    listenning_ = true;
    // 设置acceptSocket_的监听上限
    acceptSocket_.listen();
    // 将acceptSocket_期望监听的事件设置为读事件
    acceptChannel_.enableReading();
}

// 当acceptSocket_上有读事件发生时，就进行该回调 (通过accept()来接收对端的connfd)。
void Acceptor::handleRead()
{
    LOG_INFO("func:%s happened\n", __func__);
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(peerAddr);//把peerAddr绑定成对端的网络地址
    if (connfd > 0)
    {
        //newConnectionCallBack_=>TcpServer::newConnection
        // newConnectionCallBack_=>把connfd打包成channel然后分发给subloop进行监听
        if (newConnectionCallBack_)
        {
            newConnectionCallBack_(connfd, peerAddr);
        }
        else
        {
            ::close(connfd);
        }
    }
    else
    {
        LOG_WARN("%s:%s:%d accept error \n", __FILE__, __FUNCTION__, __LINE__);
        if (errno == EMFILE)
        {
            LOG_WARN("Connected fd_nums reached limited!\n");
        }
    }
}
