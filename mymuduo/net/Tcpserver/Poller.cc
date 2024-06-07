#include "Poller.h"
#include "Channel.h"

Poller::Poller(EventLoop *loop)
    : ownerLoop_(loop)
{
}
//判断channel是否属于该poller
bool Poller::hasChannel(Channel *channel) const
{
    auto it = channels_.find(channel->fd());
    return it == channels_.end() && it->second == channel;
}
