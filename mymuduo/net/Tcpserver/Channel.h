#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "noncopyable.h"
#include "Timestamp.h"

#include <memory>
#include <functional>

class EventLoop;

/*
 *Channel理解为通道，其中封装了socketfd和感兴趣的事件events，如EPOLLIN,EPOLLOUT
 *还绑定了epoll中返回的事件
 */
class Channel
{
public:
    using ReadEventCallback = std::function<void(Timestamp)>;
    using EventCallback = std::function<void()>;
    explicit Channel(EventLoop *loop, int fd);

    // 设置具体的处理事件，给epoll调用
    void handleEvent(Timestamp recv_Time);
    void setReadCallback(ReadEventCallback cb)
    {
        readCallback_ = std::move(cb);
    }
    void setWriteCallback(EventCallback cb)
    {
        writeCallback_ = std::move(cb);
    }
    void setCloseCallback(EventCallback cb)
    {
        closeCallback_ = std::move(cb);
    }
    void setErrorCallback(EventCallback cb)
    {
        errorCallback_ = std::move(cb);
    }

    /// 将此通道绑定到shared_ ptr管理的所有者对象，
    /// 防止当channel被手动remove掉，channel还在执行回调操作
    void tie(const std::shared_ptr<void> &);

    int fd() const { return fd_; }
    int events() const { return events_; }              // 返回当先fd中想要监听的事件类型
    void set_revents(int revent) { revents_ = revent; } // 在poller中设置监听到的事件类型

    // 设置fd的相应的事件状态
    void enableReading() { events_ |= kReadEvent, update(); }
    void disableReading() { events_ &= ~kReadEvent, update(); }
    void enableWriting() { events_ |= kWriteEvent, update(); }
    void disableWriting() { events_ &= ~kWriteEvent, update(); }
    void disableAll() { events_ = kNoneEvent, update(); }

    // 判断fd当前想要注册的事件状态：无事件，可读事件，可写事件
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isReading() const { return events_ & kReadEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }

    // for poller
    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }

    EventLoop *ownerLoop() { return loop_; } // 返回通道所在的事件分发器
    void remove();

private:
    // 事件类型
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    void update(); // epoll_ctl
    void handleEventWithGuard(Timestamp recv_Time);

    EventLoop *loop_; // 事件循环
    const int fd_;    // fd ，监听的对象
    int events_;      // 注册fd想要监听的事件类型,
    int revents_;     // 收到poller所监听到fd上发生的事件
    int index_;       // 标志着channel在poller中对应的状态

    std::weak_ptr<void> tie_;
    bool tied_;                 //弱指针的绑定状态
    /*
     *bool eventHandling; （在channel的析构时，判断是否还在处理回调）
     *bool addToLoop;（在channel的析构时 ，判断channel是否已经从事件分发器loop中移除，即这个fd不需要再被监听了）
     */

    // 所监听的事件
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;
};

#endif