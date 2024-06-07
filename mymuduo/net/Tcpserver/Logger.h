#ifndef _LOGGER_H_
#define _LOGGER_H_

#include"noncopyable.h"
#include"Timestamp.h"

#include <iostream>

// 枚举类型（日志输出的几个级别）:error,debug,info,warn
enum LogLevel
{
    ERROR, // 程序运行时出现错误，直接退出程序
    DEBUG, // 调试信息
    INFO,  // 正常信息
    WARN,  // 程序运行时出现错误，发出警告不退出程序
};

// 创建一个日志类，且只需要一个实例，所以可以使用懒汉模式
class Logger :public noncopyable
{
public:
    // 获取单个日志实例
    static Logger *getInstance();
    // 设置日志等级
    void setLevel(int LogLevel);
    // 输出日志
    void log(std::string msg);

private:
    Logger() = default;
    ~Logger() = default;

    //记录日志的状态
    int level_;
};

//定义日志的宏接口，方便用户使用
// exit(EXIT_FAILURE) 程序异常退出
#define LOG_ERROR(LogFormat,...)\
    do\
    { \
        Logger* m_logger=Logger::getInstance(); \
        m_logger->setLevel(ERROR); \
        char buf[1024]; \
        snprintf(buf,1024,LogFormat,##__VA_ARGS__); \
        m_logger->log(buf); \
        exit(EXIT_FAILURE); \
    } while (0);

#define LOG_WARN(LogFormat,...)\
    do\
    { \
        Logger* m_logger=Logger::getInstance(); \
        m_logger->setLevel(WARN); \
        char buf[1024]; \
        snprintf(buf,1024,LogFormat,##__VA_ARGS__); \
        m_logger->log(buf); \
    } while (0);   

#define LOG_INFO(LogFormat,...)\
    do\
    { \
        Logger* m_logger=Logger::getInstance(); \
        m_logger->setLevel(INFO); \
        char buf[1024]; \
        snprintf(buf,1024,LogFormat,##__VA_ARGS__); \
        m_logger->log(buf); \
    } while (0);  
//因为调试的日志输出会很多，先把调试的输出隐藏起来，只有当管理者需要调用调式日志的时候才能使用
#ifdef Mananger_Debug
#define LOG_DEBUG(LogFormat,...)\
    do\
    { \
        Logger* m_logger=Logger::getInstance(); \
        m_logger->setLevel(DEBUG); \
        char buf[1024]; \
        snprintf(buf,1024,LogFormat,##__VA_ARGS__); \
        m_logger->log(buf); \
    } while (0);
#else  
    #define LOG_DEBUG(LogFormat,...)
#endif


#endif