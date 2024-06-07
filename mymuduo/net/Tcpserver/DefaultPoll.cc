#include "Poller.h"
#include"EPollPoller.h"

Poller *Poller::newDefaultPoller(EventLoop *loop)
{
    if (::getenv("MUDO_USE_POLL"))//如果环境变量中有“MUDO_USE_POLL”，则默认的poller设置为poll
    {
        return nullptr;
    }
    else
    {
        return new EPollPoller(loop);
    }
}