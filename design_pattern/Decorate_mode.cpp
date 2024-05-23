#include<iostream>
#include<string>
using namespace std;
//装饰模式，顾名思义就是实现了在运行时动态扩展对象功能的能力，
//就像装修一样是附着在房子的框架上的，装饰模式是一种结构型的设计模式
//解决“主题类在多个方向上的扩展功能”——是为"装饰"的含义
//主体对象：数据流————衍生出网络流，文件流，内存流
class Stream
{
protected:
    string m_name;
public:
    Stream(){}
    Stream(string name):m_name(name){}
    virtual void show()
	{}
    virtual ~Stream(){}
};
class FileStream : public Stream
{
public:
    FileStream(string name):Stream(name){}
    void show() override
    {
        cout<<"这是文件流:"<<m_name<<endl;
    };
};
class NetworkStream: public Stream
{
public:
    NetworkStream(string name):Stream(name){}
    void show() override
    {
        cout<<"这是网络流:"<<m_name<<endl;
    };
};
class MemoryStream: public Stream
{
public:
    MemoryStream(string name):Stream(name){}
    void show() override
    {
        cout<<"这是内存流:"<<m_name<<endl;
    };
};
//对流功能的扩展
class DecorateSteam :public Stream  //通过继承来规范虚函数的接口，同时使扩展的功能可以嵌套使用
{
protected:
    Stream *m_stream;               //通过组合使用主题对象中的类   
public:
    DecorateSteam(Stream *st):m_stream(st){}
    virtual ~DecorateSteam(){}
};

class EncipherSteam :public DecorateSteam
{
public:
    EncipherSteam(Stream *st):DecorateSteam(st){}
    virtual void work()
    {
        cout<<"对流进行加密"<<endl;
        m_stream->show();
    }
};
class BufferSteam :public DecorateSteam
{
public:
    BufferSteam(Stream *st):DecorateSteam(st){}
    virtual void work()
    {
        cout<<"对流进行缓存"<<endl;
        m_stream->show();
    }
};
int main()
{
    Stream *s1=new FileStream("s1");
    Stream *s22=new NetworkStream("s22");
    EncipherSteam *s2=new EncipherSteam(s22);
    s2->work();
    BufferSteam *s3=new BufferSteam(s1);
    s3->work();
    s2->work();
}


