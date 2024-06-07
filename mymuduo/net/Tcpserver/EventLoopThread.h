#ifndef _EVENTLOOPTHREAD_H
#define _EVENTLOOPTHREAD_H

#include <functional>
#include <mutex>
#include <condition_variable>

#include "EventLoop.h"
#include "Thread.h"

/*
* subreactor：
*             EventLoopThread (相当于一个管理者，通过它的子线程来实现对事件的循环监听) 
*             son_thread( EventLoop )       
* 这也符合着one_loop=>per thread,即一个子线程只拥有一个事件分发器loop。                                   
*/
class EventLoopThread : noncopyable
{
public:
    //对子线程的EventLoop进行指定初始化的函数
    using ThreadInitCallBack = std::function<void(EventLoop *)>;
    //const ThreadInitCallBack &cb = ThreadInitCallBack()的作用是使cb的默认赋值为空。
    EventLoopThread(const ThreadInitCallBack &cb = ThreadInitCallBack()
                    , const std::string &name = std::string());
    ~EventLoopThread();
    // 创建子线程，并让子线程开始监听事件的发生
    EventLoop *startLoop();

private:
    void threadFunc(); // 给创建新的子线程执行的回调函数

    EventLoop *loop_; //指向子线程的EventLoop变量
    bool exiting_;    //标识结束子线程的事件监听
    Thread thread_;   //操控子线程的创建和回收
    std::mutex mutex_;
    std::condition_variable cond_;
    ThreadInitCallBack CallBack_;//当需要对子线程中的EventLoop进行额外的初始化时才调用
};

#endif