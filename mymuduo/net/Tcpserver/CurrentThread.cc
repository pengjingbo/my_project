#include "CurrentThread.h"

namespace CurrentThread
{
    thread_local int t_catchedTid = 0;

    void catchedTid()
    {
        if (t_catchedTid == 0)
        {
            // 通过linux的系统调用，获取当前线程的tid
            t_catchedTid = static_cast<pid_t>(::syscall(SYS_gettid));
        }
    }
}
