#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include "Timestamp.h"

#include <memory>
#include <functional>
#include <sys/types.h>

class Buffer;
class TcpConnection;
namespace net
{
    // 指向tcpconnection的智能指针
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    /*
     *下面的回调函数都是用户可自定义的回调函数
     */
    using TimerCallback = std::function<void()>;

    // 由用户设置的有新的连接生成或者断开时就会执行的回调
    using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;

    // 由由用户设置的关闭连接的时候就会执行的函数
    using CloseCallback = std::function<void(const TcpConnectionPtr &)>;

    // 由用户设置的向对端写完数据立即就会执行的函数
    using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;

    using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr &, size_t)>;

    // 由用户设置的接收到数据时就会执行的回调函数
    using MessageCallback = std::function<void(const TcpConnectionPtr &, Buffer *, Timestamp)>;

    // 用户如果未指定的话使用的默认回调函数
    void defaultConnectionCallback(const TcpConnectionPtr &conn);
    void defaultMessageCallback(const TcpConnectionPtr &, Buffer *buffer, Timestamp receiveTime);
}

#endif