#include "EventLoopThread.h"

EventLoopThread::EventLoopThread(const ThreadInitCallBack &cb, const std::string &name)
    : loop_(nullptr)
    , exiting_(false)
    , thread_(std::bind(&EventLoopThread::threadFunc, this), name)//绑定子线程创建时执行的回调函数
    , mutex_()
    , cond_()
    , CallBack_(cb)
{
}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    // 当子线程中的loop还在运行时，才需要让loop退出，然后回收子线程
    if (loop_ != nullptr)
    {
        loop_->quit();
        thread_.join();
    }
}

// 创建子线程，开始事件循环
EventLoop *EventLoopThread::startLoop()
{
    thread_.start(); // 创建一个子线程，并让其执行threadFunc()
    EventLoop *loop = nullptr;
    {
        // 等待子线程在进行threadFunc(),将它的eventloop传递回来
        std::unique_lock<std::mutex> locker(mutex_);
        while (loop_ == nullptr)
        {
            cond_.wait(locker);
        }
        loop = loop_;
    }
    return loop;
}
// 子线程创建时执行的回调函数
void EventLoopThread::threadFunc()
{
    EventLoop loop;
    // 所有线程共享一个地址空间，所以子线程可以访问父线程中的变量
    if (CallBack_)
    {
        // 对子线程中的loop进行一些额外的初始化
        CallBack_(&loop);
    }
    {
        std::lock_guard<std::mutex> locker(mutex_);
        loop_ = &loop; // 将在子线程中创建的EventLoop对性的地址返回给父线程
        cond_.notify_one();
    }
    // 子线程在一个独立的线程中执行着EventLoop中的loop（监听channel上是否有事件发生）
    loop.loop();
    std::lock_guard<std::mutex> locker(mutex_);
    loop_ = nullptr;
}