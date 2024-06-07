#include "Logger.h"
Logger *Logger::getInstance()
{
    static Logger logger;
    return &logger;
}
void Logger::setLevel(int LogLevel)
{
    level_ = LogLevel;
}

// 日志的输出格式：[LogLevel] time : msg
void Logger::log(std::string msg)
{
    switch (level_)
    {
    case ERROR:
        std::cout << "[ERROR] ";
        break;
    case WARN:
        std::cout << "[WARN] ";
        break;
    case INFO:
        std::cout << "[INFO] ";
        break;
    case DEBUG:
        std::cout << "[DEBUG] ";
        break;
    default:
        break;
    }
    std::cout << Timestamp::now().ToString() << " : " << msg << std::endl;
}


/* int main()
{
    LOG_ERROR("%s", "这是一条测试日志的输出信息");
    LOG_DEBUG("%s", "这是一条测试日志的输出信息");
    LOG_INFO("%s", "这是一条测试日志的输出信息");
    LOG_WARN("%s", "这是一条测试日志的输出信息");
} */