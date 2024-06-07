#include "EPollPoller.h"
#include "Logger.h"
#include "Channel.h"

#include <errno.h>
#include <assert.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>

// 标志channel的状态
const int kNew = -1;    // 还未加入中epoll中
const int kAdded = 1;   // 已经加入到epoll中
const int kDeleted = 2; // 已经从epoll中删除，但还没从channel_map从删除

EPollPoller::EPollPoller(EventLoop *loop)
    : Poller(loop), Epollfd_(::epoll_create1(EPOLL_CLOEXEC)) // EPOLL_CLOEXEC 当执行execve创建新进程时，打开的描述符自动关闭,
    , events_(kInitEventListSize)                           //简单说就是fork子进程时不继承，对于多线程的程序设上这个值不会有错的。
{
    if (Epollfd_ < 0)
    {
        LOG_ERROR("EPollPoller create error : %d\n", errno);
    }
}
EPollPoller::~EPollPoller()
{
    ::close(Epollfd_);
}
// epoll_wait()
Timestamp EPollPoller::poll(int timeoutMS, ChannelList *activeChannels)
{
    LOG_DEBUG("func:%s => fd total count:%lu\n", __func__, channels_.size());
    // &*events_.begin() 返回存放epoll_event的数组的首地址
    int numEvents = ::epoll_wait(Epollfd_, &*events_.begin(), static_cast<int>(events_.size()),timeoutMS);
    int savedErrno = errno;
    Timestamp now(Timestamp::now());
    if (numEvents > 0)
    {
        // 将有事件发生的channel添加到eventloop中的activeChannels
        fillActiveChannels(numEvents, activeChannels);
        // 如果有事件发生的channel数量==记录事件发生fd的events_的大小，需要扩展events_的大小
        if (static_cast<size_t>(numEvents) == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if (numEvents == 0)
    {
        LOG_DEBUG("nothing happened!\n");
    }
    else
    {
        // 如果错误类型不为外部中断，用日志输出错误信息，如果是外部中断，不影响程序的继续执行。
        if (savedErrno != EINTR)
        {
            errno = savedErrno;
            LOG_WARN("epoll_wait has an errno : %d\n", errno);
        }
    }

    return now;
}

// epoll_ctl
void EPollPoller::update(int operation, Channel *channel) const
{
    struct epoll_event event;

    bzero(&event,sizeof(event));
    event.events = channel->events(); // 注册channel想要监听的事件
    event.data.ptr = channel;
    int fd = channel->fd();
    // 如果epoll_ctl出现错误s
    if (::epoll_ctl(Epollfd_, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {
            LOG_WARN("EPOLL_CTL_DEL operation error : %d\n", errno);
        }
        else if (operation == EPOLL_CTL_ADD)
        {
            LOG_ERROR("EPOLL_CTL_ADD operation error : %d\n", errno);
        }
        else
        {
            LOG_ERROR("EPOLL_CTL_MOD operation error : %d\n", errno);
        }
    }
}
void EPollPoller::updateChannel(Channel *channel)
{
    const int index = channel->index(); // 获取channel的状态
    int fd = channel->fd();
    LOG_INFO("func:%s => index=%d ,fd=%d ,events=%d \n", __func__, index, fd, channel->events());
    // 如果channel之前已经被epoll删除，或者还没被加入过epoll
    if (index == kNew || index == kDeleted)
    {
        // channel没被加入过epoll,更新channel_map
        if (index == kNew)
        {
            channels_[fd] = channel;
        }
        //改变channel的连接状态
        channel->set_index(kAdded); 
        update(EPOLL_CTL_ADD, channel);
    }
    else // 当前channel已经在epoll中了，就只需要判断是更改监听事件还是删除该channel
    {
        if (channel->isNoneEvent()) // 当前channel无想要注册的监听事件，可以从epoll中删除
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}
// 要从epoll和channel_map中同时删除
void EPollPoller::removeChannel(Channel *channel)
{
    const int index = channel->index(); // 获取channel的状态
    int fd = channel->fd();
    LOG_INFO("func:%s => index=%d ,fd=%d ,events=%d \n", __func__, index, fd, channel->events());
    if (channels_.find(fd) != channels_.end())
    {
        channels_.erase(fd);
    }

    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    // 更新channel的状态
    channel->set_index(kNew);
}

// 将发生事件的channel添加到eventloop中的activeChannels中
void EPollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
    std::cout<<numEvents<<std::endl;
    for (int i = 0; i < numEvents; i++)
    {
        Channel* channel = static_cast<Channel *>(events_[i].data.ptr);
        LOG_INFO("func:%s => channel fd=%d \n", __func__,channel->fd());
        int fd = channel->fd();
        
        ChannelMap::const_iterator it = channels_.find(fd);
        assert(it != channels_.end());
        assert(it->second == channel);
        channel->set_revents(events_[i].events); // 设置channel的回调事件
        activeChannels->push_back(channel);
    }
}

