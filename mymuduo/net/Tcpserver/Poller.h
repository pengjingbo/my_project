#ifndef _POLLER_H_
#define _POLLER_H_

#include <vector>
#include <unordered_map>

#include "EventLoop.h"
#include "Timestamp.h"

class Channel;
class EventLoop;

/*
 *muduo多路事件分发器的核心IO复用模块
 */
class Poller
{
public:
    using ChannelList = std::vector<Channel *>;
    Poller(EventLoop *loop);
    virtual ~Poller(){}

    // 给所有的IO复用保留统一的接口
    virtual Timestamp poll(int timeoutMS, ChannelList *activeChannels) = 0;
    /// Changes the interested I/O events.
    /// Must be called in the loop thread.
    virtual void updateChannel(Channel *channel) = 0;
    /// Remove the channel, when it destructs.
    /// Must be called in the loop thread.
    virtual void removeChannel(Channel *channel) = 0;
    virtual bool hasChannel(Channel *channel) const;

    static Poller *newDefaultPoller(EventLoop *loop);

protected:
    // Map的key代表着channel中的fd，value则代表着fd所对应channel的实例
    //channels_中记录着曾被加入到epoller中的socket，包括活跃和非活跃的socket
    using ChannelMap = std::unordered_map<int, Channel *>;
    ChannelMap channels_;

private:
    EventLoop *ownerLoop_; // poller所属的事件循环对象
};


#endif
