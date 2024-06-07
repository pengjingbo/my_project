#ifndef _EVENTLOOPTHREADPOOL_H
#define _EVENTLOOPTHREADPOLL_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "EventLoop.h"
#include "EventLoopThread.h"
#include "noncopyable.h"

// 相当于一个线程池，里面存储着多个subreactor和事件分发器EventLoop
class EventLoopThreadPool : noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;

    EventLoopThreadPool(EventLoop *baseLoop, const std::string &nameArg = std::string());
    ~EventLoopThreadPool();

    // 设定创建的subreactor的数量
    void setThreadNum(int Threadnums) { numThreads_ = Threadnums; }
    // 线程池开始工作
    void start(const ThreadInitCallback &cb = ThreadInitCallback());
    // 如果工作在多线程下，EventLoopThreadPool默认将会以轮询的方式将channel分配给子subloop
    EventLoop *getNextLoop();

    // 返回loops_中的所有loop
    std::vector<EventLoop *> getAllLoops();
    // 判断mainreactor是否开始工作
    bool started() const { return started_; }
    // 返回mainreactor的名字
    const std::string &name() const { return name_; }

private:
    EventLoop *baseLoop_;                                   // 如果没有设置线程池中线程数量的话baseLoop_就是mainloop
    std::string name_;                                      // baseLoop_的name
    bool started_;                                          // 标识线程池是否开始工作
    int numThreads_;                                        // 记录创建的子线程(subloop)的数量
    int next_;                                              // 用于轮询时访问loops_数组的下表
    std::vector<std::unique_ptr<EventLoopThread>> threads_; // 存放子线程(subreactor)
    std::vector<EventLoop *> loops_;                        // 存放每一个子线程的EventLoop
};

#endif