#include "Thread.h"
#include"CurrentThread.h"
#include "Logger.h"

#include <semaphore.h>

std::atomic_int32_t Thread::numCreated_ (0);

Thread::Thread(ThreadFunc func, const std::string &name)
    : started_(false)
    , joined_(false)
    , thread_(nullptr)
    , tid_(0)
    , func_(std::move(func))
    , name_(name)
{
    setDefaultName();
}
Thread::~Thread()
{
    // 如果线程已经创建了，并且没有被join()回收
    if (started_ && !joined_)
    {
        thread_->detach();
    }
}

void Thread::start()
{
    started_ = true;
    //使用信号量是为了确保主线程在创建新线程时，能够让tid_成功记录子线程的线程id
    //而不是子线程还未完成tid_=CurrentThread::tid()时，主线程已经结束了start()
    sem_t sem;
    sem_init(&sem,false,0);
    // 创建线程并执行相应函数   
    thread_=std::make_shared<std::thread>([&](){
        tid_=CurrentThread::tid();
        sem_post(&sem);
        func_();
    });
    //主线程阻塞，等待tid记录子线程的线程id,避免调用tid()时，新线程的tid还没有被初始化
    sem_wait(&sem);
}
// 执行线程的回收
void Thread::join()
{
    joined_ = true;
    thread_->join();
}

void Thread::setDefaultName()
{
    int num = ++numCreated_;
    // 线程的默认名字为第几个线程——"Thread%d"
    if (name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = buf;
    }
}