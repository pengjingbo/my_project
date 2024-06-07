#include "TcpServer.h"
#include "TcpConnection.h"
#include "Logger.h"

using namespace std::placeholders;
// 检查mainloop是否为空
static EventLoop *CheckLoopNotNull(EventLoop *loop)
{
    if (loop == nullptr)
    {
        LOG_ERROR("%s:%s:%d mainloop is null!\n", __FILE__, __FUNCTION__, __LINE__);
    }
    return loop;
}

TcpServer::TcpServer(EventLoop *loop
                    , const InetAddress &listenAddr
                    , const std::string nameArg
                    , Option option )
                    : loop_(CheckLoopNotNull(loop))
                    , ipPort_(listenAddr.toIpPort())
                    , name_(nameArg)
                    , acceptor_(new Acceptor(loop,listenAddr,option==kReusePort))
                    , threadpool_(new EventLoopThreadPool(loop,nameArg))
                    , connectionCallback_(defaultConnectionCallback)
                    , messageCallback_(defaultMessageCallback)
                    , started_(0)
                    , nextConnId_(1)
{
    //设置acceptor_中的NewConnectionCallBack函数
    acceptor_->setNewConnectionCallBack(
        std::bind(&TcpServer::newConnection,this,_1,_2));

}

TcpServer::~TcpServer()
{
    for(auto& item:connections_)
    {
        //这里使用局部变量临时保存item.second所指向的TcpConnection对象，这样在item.second.reset()后
        //还是可以通过局部对象来访问connectDestroyed，在函数执行后局部变量所管理的资源也会被释放
        TcpConnectionPtr conn(item.second);
        //释放ConnectionMap对象中的资源
        item.second.reset();
        //
        conn->getLoop()->runInLoop(
            std::bind(&TcpConnection::connectDestroyed,conn)
        );
    }
}

void TcpServer::setThreadNum(int numThreads)
{
    threadpool_->setThreadNum(numThreads);
}

void TcpServer::start()
{
    if (started_++ == 0) // 避免mainloop被start()多次
    {
        // 启动subloop池，创建numThreads个subreactor。
        threadpool_->start(threadInitCallback_);
        // 在监听是否有新连接产生时，先设置listenfd的监听上限
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
        // 用户端调用loop_->loop()，监听新连接的产生，有的话就会执行Acceptor中的handleRead()。
    }
}

//当有新用户连接时，poller=>notify acceptor执行这个回调函数
void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
{
    // 通过轮询算法，获得一个subreactor来监听新连接
    EventLoop *ioLoop = threadpool_->getNextLoop();
    // 给TcpConnection设置名字
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    std::string connName = name_ + buf;

    TcpConnectionPtr conn(new TcpConnection(ioLoop
                                         , connName
                                         , sockfd
                                         , peerAddr
                                         , peerAddr));
    connections_[connName] = conn;

    /* 下面的回调都是由用户设置给TcpConnection的*/

    // 连接成功或者连接断开的时候执行的回调函数
    conn->setConnectionCallback(connectionCallback_);
    // 接收到对端有数据发送时执行的回调函数
    conn->setMessageCallback(messageCallback_);
    // 当向对端发送完数据时执行的回调函数
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    // 当连接关闭时执行的回调函数
    conn->setCloseCallBack(
        std::bind(&TcpServer::removeConnection, this, _1));

    //把新连接的TcpConnection设置完成后，执行TcpConnection=>connectEstablished()
    ioLoop->runInLoop(
        std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    loop_->runInLoop(
        std::bind(&TcpServer::removeConnectionInLoop,this,conn)
    );
}
//删除connections_中对应的TcpConnectionPtr，然后再执行TcpConnection::connectDestroyed
void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
    LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s\n"
                                ,name_.c_str()
                                ,conn->name().c_str());
    // 从ConnectionMap删除相应的TcpConnectionPtr，这样在整个程序中就只有当前函数中的conn
    // 指向要删除的TcpConnection对象，在函数结束后share_ptr的引用计数变为0，指向的内存将会自动释放                         
    connections_.erase(conn->name()); 
    EventLoop *ioLoop = conn->getLoop();

    // 再调用TcpConnection::connectDestroyed,来移除subloop中监听的channel
    ioLoop->queueInLoop(
        std::bind(&TcpConnection::connectDestroyed, conn));
}

int main()
{
    return 0;
}
