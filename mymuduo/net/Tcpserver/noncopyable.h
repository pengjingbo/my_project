#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_
class noncopyable
{

public:
    noncopyable(const noncopyable &)=delete;
    noncopyable operator= (const noncopyable &)=delete;
protected:
    noncopyable()=default;    
    ~noncopyable()=default;
};

#endif