#ifndef _THREAD_H
#define _THREAD_H

#include <functional>
#include <string>
#include <atomic>
#include <memory>
#include <thread>

#include "noncopyable.h"

//一个Thread对象记录着一个新线程的所有信息，新线程(子线程)。
class Thread : noncopyable
{
public:
    using ThreadFunc = std::function<void()>;
    //用 std::string()来初始化name，如果构造是没有传入具体的string类型的参数，则设置name为空
    explicit Thread(ThreadFunc, const std::string &name = std::string());
    ~Thread();

    // 创建子线程，让其执行回调函数
    void start();
    // 执行新线程的回收
    void join(); // return thread.join()

    // 判断新线程是否已经开始工作
    bool started() const { return started_; }
    // 返回新线程的tid；
    pid_t tid() const { return tid_; }
    // 返回新线程的名字
    const std::string &name() const { return name_; }
    // 返回全局中创建了的新线程的个数
    static int numCreated() { return numCreated_.load(); }

private:
    void setDefaultName(); // 设置新线程的默认名字

    bool started_; // 标识新线程是否被创建
    bool joined_;  // 标识新线程是否被主线程回收
    std::shared_ptr<std::thread> thread_;//指向子线程的智能指针
    int tid_;          // 当前新线程的线程id
    ThreadFunc func_;  // 新线程被创建后进行的回调函数
    std::string name_; // 新线程的名字

    static std::atomic_int32_t numCreated_; // 记录全局的创建的线程数
};

#endif
