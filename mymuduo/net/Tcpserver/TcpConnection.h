#ifndef _TCPCONNECTION_H_
#define _TCPCONNECTION_H_

#include "Callbacks.h"
#include "Buffer.h"
#include "noncopyable.h"
#include "InetAddress.h"

#include <memory>
#include <atomic>
#include <string>

class Channel;
class EventLoop;
class Socket;

using namespace net;
/*
*TcpConnection负责对一个客户端的channel事件的管理(读写事件的回调函数，连接断开和连接建立时的回调函数，连接出现错误时的回调函数)
*    ConnectionCallback connectionCallback_;
*    MessageCallback messageCallback_;
*    WriteCompleteCallback writeCompleteCallback_;
*    HighWaterMarkCallback highWaterMarkCallback_;
*    CloseCallback closeCallback_;
*上面这些都是可以由用户设置的回调函数，这些回调函数不会影响channel-sockfd本身的业务处理逻辑
*   (void handleRead(Timestamp receiveTime);
*    void handleWrite();
*    void handleClose();
*    void handleError();）
*简单的说，用户可以不用管理服务端对数据读写的具体操作(即怎么read()和怎么write()才不会使数据传输过程中出现异常)
*，这些都由TcpConnection中的上面四个函数负责处理了
*/
class TcpConnection : noncopyable,
                      // 为了能够返回管理this的指针
                      public std::enable_shared_from_this<TcpConnection>
{
public:
    explicit TcpConnection(EventLoop *loop
                , const std::string &nameArg
                , int sockfd
                , const InetAddress &localAddr
                , const InetAddress &peerAddr);
    ~TcpConnection();
    EventLoop *getLoop() const { return loop_; }
    const std::string &name() { return name_; }
    const InetAddress &localAddress() { return localAddr_; }
    const InetAddress &peerAddress() { return peerAddr_; }
    bool connected() { return state_ == kConnected; }
    bool disconnected() { return state_ == kDisconnected; }

    // 给connfd发送数据,也就是触发connfd的写事件，(服务器本身是不会给客户端发送数据的,只有用户端手动调用这个函数的时候才会，监听connfd的写事件)
    void send(const std::string&message);
    // close connfd
    void shutdown();

    // 设置回调函数
    void setConnectionCallback(const ConnectionCallback &cb)
    {
        connectionCallback_ = cb;
    }
    void setMessageCallback(const MessageCallback &cb)
    {
        messageCallback_ = cb;
    }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    {
        writeCompleteCallback_ = cb;
    }
    void setHighWaterMarkCallback(const HighWaterMarkCallback &cb, size_t highWaterMark)
    {
        highWaterMark_ = highWaterMark, highWaterMarkCallback_ = cb;
    }
    void setCloseCallBack(const CloseCallback &cb)
    {
        closeCallback_ = cb;
    }

    void connectEstablished();
    void connectDestroyed();

private:
    enum StateE
    {
        kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting
    };
    void setState(StateE state) { state_ = state; }

    void sendInLoop(const void *data, size_t len);
    void shutdownInLoop();

    //给客户端connfd的channel设置的事件发生后channel将会执行的回调函数
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

private:
    EventLoop *loop_; // channel所派发给的subloop
    const std::string name_;
    std::atomic_int state_; // tcp的连接状态
    bool reading_;

    std::unique_ptr<Socket> socket_;   // 客户端的connfd
    std::unique_ptr<Channel> channel_; // 封装了connfd和发生事件的回调
    const InetAddress localAddr_;      // 本地的网络结构
    const InetAddress peerAddr_;       // 对端的网络结构

    
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    HighWaterMarkCallback highWaterMarkCallback_;
    CloseCallback closeCallback_;

    size_t highWaterMark_; // 高水位界限

    // 接收数据和发送数据给对端时所用的缓冲区
    Buffer inputBuffer_;   
    Buffer outputBuffer_;
};

#endif