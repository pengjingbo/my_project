#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

#include <iostream>
#include <time.h>
class Timestamp
{
public:
    Timestamp();
    explicit Timestamp(time_t time);
    static Timestamp now(); //返回timestamp的单个实例
    std::string ToString();
    ~Timestamp() = default;

private:
    time_t seconds_;
};

#endif