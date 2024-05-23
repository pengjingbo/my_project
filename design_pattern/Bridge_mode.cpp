#include<iostream>
using namespace std;
//桥接模式，顾名思义，需要有一个桥梁（即抽象类中的一个指向实现类的指针）
//抽象类和实现类就是通过这个桥，使得两个类可以独立的沿着各自的维度来变化，但又能通过桥来进行组合产生联系
//例如手机有很多种厂家，相应的操作系统也有很多的版本，不同款式的手机适用的操作系统也不一样，这时候，手机的类型就作为抽象，不
//同的操作系统就可以看成作实现，手机类通过一个指向操作系统类（实现）的指针，在运行时绑定一个具体的操作系统
//抽象类（这里的抽象指的是变化不大的接口，而实现指的是易变化的类）
//实现类
class OS
{
public:
    virtual void shown()=0;
    virtual ~OS(){}
};
class IOS:public OS
{
public:
    void shown()
    {
        cout<<"搭载了ios的操作系统\n";
    }
};
class ColorOS:public OS
{
public:
    void shown()
    {
        cout<<"搭载了ColorOS的操作系统\n";
    }
};
class One_Ul:public OS
{
public:
    void shown()
    {
        cout<<"搭载了One_Ul的操作系统\n";
    }
};

class Phone
{
protected:
    OS*m_os;
public:
    Phone(OS *os):m_os(os){}
    virtual void show()=0;
    virtual ~Phone(){}
};
class apple :public Phone
{
public:
    apple(OS *os):Phone(os){}
    void show()
    {
        cout<<"这是一台苹果手机\n";
        m_os->shown();
    }
};
class oppo :public Phone
{
public:
    oppo(OS *os):Phone(os){}
    void show()
    {
        cout<<"这是一台oppo手机\n";
        m_os->shown();
    }
};
class samsang :public Phone
{
public:
    samsang(OS *os):Phone(os){}
    void show()
    {
        cout<<"这是一台samsang手机\n";
        m_os->shown();
    }
};

int main()
{
    Phone *m_phone=new apple(new ColorOS);
    m_phone->show();
    delete m_phone;
}


