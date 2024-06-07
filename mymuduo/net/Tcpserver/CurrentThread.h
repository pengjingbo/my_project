#ifndef _CURRENTTHREAD_H_
#define _CURRENTTHREAD_H_

#include<unistd.h>
#include<sys/syscall.h>

namespace CurrentThread
{
    extern thread_local int t_catchedTid;//头文件中声明一个每个线程都有的线程id
    void catchedTid();

    //返回当前线程的线程id
    inline int tid()
    {
        //__builtin_expect是让编译器对ifelse判断分支语句进行优化
        if(__builtin_expect(t_catchedTid==0,0))
        {
            catchedTid();
        }
        return t_catchedTid;
    }
}




#endif
