#include "Logger.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"

#include <sys/eventfd.h>
#include <unistd.h>

// 用来标识当前线程是否已经拥有了一个EventLoop，在eventLoop的构造函数中进行判断
thread_local EventLoop *t_loopInThisThread = 0;
// poller中的超时时间
const int kPollTimeMs = 10000;

int createEventFd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_WARN("Failed in create an eventfd\n");
    }
    return evtfd;
}

EventLoop::EventLoop()
        : looping_(false)
        , quit_(false)
        ,threadId_(CurrentThread::tid())
        ,poller_(Poller::newDefaultPoller(this))
        ,wakeupFd_(createEventFd())
        ,wakeupChannel_(new Channel(this,wakeupFd_))
        , callingPendingFunctors_(false)
{
    LOG_DEBUG("EventLoop created %p in thread %d\n", this, threadId_);
    if (t_loopInThisThread)//如果t_loopInThisThread先前已经绑定了一个EventLoop，报错并终止程序的进行
    {
        LOG_ERROR("Another EventLoop exists in this thread %d\n", threadId_);
    }
    else
    {
        t_loopInThisThread = this;
    }
    /* 
    * wakeupfd，每一个EventLoop都只需要监听它的读事件，因为要通过它来唤醒subreactor
    * wakefd的回调函数是怎样的不重要，
    * 重要的是subreactor通过监听它的读事件，使mainreactor能够唤醒自己。
    */
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop()
{
    looping_ = true;
    quit_ = false;
    LOG_INFO ("%d Thread EventLoop start looping",threadId_);
    while (!quit_)
    {
        activeChannels_.clear();
        // 监听者两种socket，一种是客户端的socket，另一种是wakeupFd
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for (Channel *channel : activeChannels_)
        {
            channel->handleEvent(pollReturnTime_);
        }
        /*
         *当mainreactor需要subreactor处理某个事件时，会先将cb准备好，
         *通过runInLoop()||queueInLoop(),添加到
         *pendingFunctors_中，然后通过wakeup()来唤醒阻塞在poller_->poll（）上的subreactor，
         *让它来处理新派发的回调事件，即doPendingFunctors()
         */
        doPendingFunctors();
    }
    LOG_DEBUG("EventLoop %p stop looping\n", this);
    looping_ = false;
}
// 当在别的线程中调用该loop的quit()方法时，需要将loop唤醒后才能成功quit
// 如果是在loop所在的线程调用这个函数，说明loop没有阻塞在poller_->poll（），直接就可以成功quit()
void EventLoop::quit()
{
    quit_ = true;
    // There is a chance that loop() just executes while(!quit_) and exits,
    // then EventLoop destructs, then we are accessing an invalid object.
    // Can be fixed using mutex_ in both places.
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        // 通过局部对象functors来保存pendingFunctors_的所有元素，
        // 避免下面操作pendingFunctors_时频繁上锁带来的性能开销
        std::lock_guard<std::mutex> locker(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (const Functor &functor : functors)
    {
        functor();
    }

    callingPendingFunctors_ = false;
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        LOG_ERROR("EventLoop::handleRead() reads %lu bytes instead of 8\n", n);
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        LOG_ERROR("EventLoop::wakeup() reads %lu bytes instead of 8\n", n);
    }
}

//subloop执行mainloop派发的回调事件
void EventLoop::runInLoop(Functor cb)
{
    if(isInLoopThread())//如果调用该方法的线程是该loop所在的线程，直接执行cb
    {
        cb();
    }
    else
    {
        queueInLoop(std::move(cb));
    }
}
//最重要的重用就是能够唤醒当前EventLoop所在的线程，并给其添加新的监听事件
void EventLoop::queueInLoop(Functor cb)
{
    {
        std::lock_guard<std::mutex> locker(mutex_);
        pendingFunctors_.emplace_back(cb);
    }
    /*  ||callingPendingFunctors_的判断逻辑是如果调用该函数的线程是loop所在的线程，
    * 但loop现在刚好在执行doPendingFunctors()，即loop无法处理mainloop所派发的新回调事件
    * 所以当loop执行完doPendingFunctors()就会继续阻塞在poller_->poll()上，这个时候
    * 就需要重新唤醒loop，才能让loop能够处理新派发的回调事件
    */
    if(!isInLoopThread()|| callingPendingFunctors_)
    {
        wakeup();
    }
}


void EventLoop::updateChannel(Channel *channel)
{
    poller_->updateChannel(channel);
}
void EventLoop::removeChannel(Channel *channel)
{
    poller_->removeChannel(channel);
}
bool EventLoop::hasChannel(Channel *channel)
{
    return poller_->hasChannel(channel);
}