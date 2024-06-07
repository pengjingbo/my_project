#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include "Callbacks.h"
#include "EventLoop.h"
#include "noncopyable.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include "EventLoopThreadPool.h"

#include <unordered_map>
#include <atomic>

using namespace net;

class TcpServer : noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;
    enum Option // 设置listenfd的端口复用
    {
        kNoReusePort,
        kReusePort,
    };
    explicit TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string nameArg, Option option = kNoReusePort);
    ~TcpServer();

    const std::string &ipPort() const { return ipPort_; }
    const std::string &name() const { return name_; }
    EventLoop *getLoop() const { return loop_; }

    // 设置subloop线程数量
    void setThreadNum(int numThreads);
    // 设置subloop中EventLoop的初始化函数
    void setThreadInitCallback(const ThreadInitCallback &cb) { threadInitCallback_ = std::move(cb); }
    std::shared_ptr<EventLoopThreadPool> threadPool() { return threadpool_; }

    // 开启服务器的监听
    void start();

    // TcpServer::connectionCallback_=>TcpConnection::connectionCallback_,下面的回调函数情况一样
    void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = std::move(cb); }
    void setMessageCallback(const MessageCallback &cb) { messageCallback_ = std::move(cb); }

    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { writeCompleteCallback_ = std::move(cb); }

private:
    // Acceptor=>newConnectionCallBack_=>把对端的connfd上有事件发生时执行的回调函数设置好，然后通过轮询算法派发给EventLoopThreadPool中的subloop
    void newConnection(int sockfd, const InetAddress &peerAddr);

    void removeConnection(const TcpConnectionPtr &conn);
    void removeConnectionInLoop(const TcpConnectionPtr &conn);

    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;

    EventLoop *loop_;          // mainloop，负责监听listenfd上的事件(也就是读事件---有新的连接)
    const std::string ipPort_; // listenfd绑定的ip加port
    const std::string name_;   // 用户设置的TcpServer的名字

    std::unique_ptr<Acceptor> acceptor_;              // avoid revealing Acceptor
    std::shared_ptr<EventLoopThreadPool> threadpool_; // subreactor线程池

    ConnectionCallback connectionCallback_; // 连接事件的回调函数
    MessageCallback messageCallback_;       // 接收到对端数据的回调函数
    WriteCompleteCallback writeCompleteCallback_;//发送完数据的回调函数
    ThreadInitCallback threadInitCallback_; // 对subreactor中的EventLoop的初始化函数

    std::atomic_int32_t started_; // 标识TcpServer是否开始监听事件

    int nextConnId_;            //作TcpConnection的名字，表示为第i个建立的TcpTcpConnection
    ConnectionMap connections_; // 存放TcpConnection
};

#endif