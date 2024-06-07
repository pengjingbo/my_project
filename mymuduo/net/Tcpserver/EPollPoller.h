#ifndef _EPOLLPOLLER_H_
#define _EPOLLPOLLER_H_

#include <vector>

#include "Poller.h"

/*
 *                                                 EventLoop
 *           Channel_list (eventloop中所有的channel)       Poller
 *                                                        Channel_map <fd,channel>（存放poller中在监听的channel）
 */

class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop *loop);
    ~EPollPoller() override;

    // 给所有的IO复用保留统一的接口
    Timestamp poll(int timeoutMS, ChannelList *activeChannels) override;

    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;

private:
    static const int kInitEventListSize = 16; // 设置events_的最大长度

    // 更新channel的状态
    void update(int operation, Channel *channel) const;
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;

    using EventList = std::vector<struct epoll_event>;

    int Epollfd_;      // epoll_create1返回的epoll_fd
    EventList events_; // 返回的有事件发生的fd数组
};

#endif
