#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"

#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd) 
    : loop_(loop)
    , fd_(fd)
    , events_(0)
    , revents_(0)
    , index_(-1)//channel在poller中的状态初始化为knew
    , tied_(false)
{
}
//一个TcpConnection创建一个新的连接时，通过channel的tie记录上层管理它TcpConnection的share_ptr;
//目的是当channel执行相应的回调函数的时候，判断它上层的TcpConnection是否被销毁。
void Channel::tie(const std::shared_ptr<void> &obj)
{
    tie_ = obj;
    tied_ = true;
}
// EventLoop中有 Channel和 poller 两个模块
void Channel::update()
{
    //调用channel所属的loop对象中update的更新方法
     loop_->updateChannel(this);
}
void Channel::remove()
{   
    //调用channel所属的loop对象中poller的remove方法
     loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp recv_Time)
{
    LOG_INFO("func:%s happened\n", __func__);
    if (tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();
        if (guard)
            handleEventWithGuard(recv_Time);
    }
    else
    {
        handleEventWithGuard(recv_Time);
    }
}
void Channel::handleEventWithGuard(Timestamp recv_Time)
{
    //EPOLLHUP表示connfd发生连接挂断
    //!(revents_ & EPOLLIN)说明服务器端不再关心connfd是否发送来了数据
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        if (closeCallback_)
            closeCallback_();
    }
    if (revents_ & EPOLLERR)
    {
        if (errorCallback_)
            errorCallback_();
    }
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLHUP))
    {
        LOG_INFO("func:%s happened\n", __func__);
        if (readCallback_)
            readCallback_(recv_Time);
    }
    if (revents_ & EPOLLOUT)
    {
        if (writeCallback_)
            writeCallback_();
    }
}
