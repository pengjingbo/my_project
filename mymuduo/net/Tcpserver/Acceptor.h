#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include <functional>

#include "Socket.h"
#include "Channel.h"
#include "noncopyable.h"

class EventLoop;
class InetAddress;
// 存活于mainloop下，负责对listenfd上的读事件进行循环的监听
class Acceptor : noncopyable
{
public:
    using NewConnectionCallBack = std::function<void(int sockfd, const InetAddress &)>;
    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallBack(const NewConnectionCallBack &cb)
    {
        newConnectionCallBack_ = std::move(cb);
    }
    // accept()
    void listen();
    // 判断Acceptor是否处于工作状态
    bool listenning() { return listenning_; }

private:
    void handleRead(); // 服务端有新的连接时调用的回调函数

    EventLoop *loop_;                             // 监听accept()事件的mainloop
    Socket acceptSocket_;                         // 需要监听accept()事件的socket
    Channel acceptChannel_;                       // acceptSocket_的channel
    NewConnectionCallBack newConnectionCallBack_; // 新连接产生后对其进行处理的函数
    bool listenning_;                             // 标识Acceptor是否出于工作状态
};

#endif