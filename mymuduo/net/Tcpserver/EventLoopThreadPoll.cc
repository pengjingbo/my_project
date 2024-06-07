#include "EventLoopThreadPool.h"
EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop
                                        ,  const std::string &nameArg)
  : baseLoop_(baseLoop),    
    name_(nameArg),
    started_(false),
    numThreads_(0),
    next_(0)
{
}
EventLoopThreadPool::~EventLoopThreadPool()
{
    // Don't delete loop, it's stack variable(栈变量)
}

//线程池的启动并完成对用户端指定数量的subloop的创建
void EventLoopThreadPool::start(const ThreadInitCallback &cb )
{
    
    started_=true;
    for(int i=0;i<numThreads_;i++)
    {
        char buf[name_.size()+32];
        snprintf(buf,sizeof buf,"%s%d",name_.c_str(),i);
        //创建一个subreactor，然后添加到subreactor的队列中
        EventLoopThread *t =new EventLoopThread(cb,buf);
        threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->startLoop());
    }
    //如果numThreads_的等于0意味着服务端只有baseloop_一个事件分发器
    if(numThreads_==0&&cb)
    {
        cb(baseLoop_);
    }
}
// 如果工作在多线程下线程池默认将会以轮询的方式将channel分配给subloop
EventLoop *EventLoopThreadPool::getNextLoop()
{
    //如果loops_.empty()为true，则返回baseloop_
    EventLoop *loop=baseLoop_;
    //轮询的机制，通过下标next_来遍历loops_数组
    if(!loops_.empty())
    {
        loop=loops_[next_];
        next_++;
        //如果next_的值大于等于了loops_.size()，就重置为0，开启一个新的循环
        if(static_cast<size_t>(next_)>=loops_.size())
        {
            next_=0;
        }
    }
    return loop;
}

// 返回loops_中的所有loop
std::vector<EventLoop *> EventLoopThreadPool::getAllLoops()
{
    if(loops_.empty())
    {
        return std::vector<EventLoop *>(1,baseLoop_);
    }else{
        return loops_;
    }
}
