#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_

#include <memory>
#include <mutex>
#include <atomic>
#include <functional>
#include <vector>

#include "Timestamp.h"
#include "noncopyable.h"
#include "CurrentThread.h"

class Channel;
class Poller;

class EventLoop :public noncopyable
{
public:
    using Functor = std::function<void()>;
    EventLoop();
    ~EventLoop();

    void loop(); // 启动事件循环
    void quit(); // 退出事件循环

    Timestamp pollReturnTime() const { return pollReturnTime_; }

    /// Runs callback immediately in the loop thread.
    /// It wakes up the loop, and run the cb.
    /// If in the same loop thread, cb is run within the function.
    /// Safe to call from other threads.
    // 在当前loop中执行cb
    void runInLoop(Functor cb);

    /// Queues callback in the loop thread.
    /// Runs after finish pooling.
    /// Safe to call from other threads.
    // 把cb放入qeuue中，唤醒loop，执行cb
    void queueInLoop(Functor cb);

    void wakeup(); // 唤醒当前subreactor线程

    // EventLoop=>poller中的方法
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);
    // 判断是否在当前线程是否是该loop所在的线程
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

private:
    void handleRead();        // waked up
    void doPendingFunctors(); // 执行mainloop给subloop派发的回调函数

    std::atomic_bool looping_; // 标识loop是否在进行事件循环
    std::atomic_bool quit_;    // 标识是否退出loop循环

    const pid_t threadId_;     // subreactor（loop）线程的线程id
    Timestamp pollReturnTime_; // poller返回的有事件发生channel的时间点

    std::unique_ptr<Poller> poller_;

    /*  
    *mainrector获取一个新用户的channel，通过轮询方法选择一个subreactor
    * 通过该成员来唤醒该subreactor 
    */
    int wakeupFd_; 
    std::unique_ptr<Channel> wakeupChannel_; // wakeupChannel_封装了wakeupfd和其感兴趣的事件。

    using ChannelList = std::vector<Channel *>;
    ChannelList activeChannels_;

    std::mutex mutex_;
    std::atomic_bool callingPendingFunctors_; // 标识当前loop是否有回调事件
    std::vector<Functor> pendingFunctors_;    // 存放loop中的回调事件
};

#endif