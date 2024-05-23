#include<iostream>
using namespace std;
//状态模式，处理的问题的类型和解决方法其实和策略模式很像，甚至可以说几乎就是同一个模式
//状态模式，为不同的状态引入不同的对象，从而使状态转化的更加明确，同时实现了具体操作和状态转化之间的解耦
class Connect_state
{
public:
    virtual void operation1()=0;
    virtual ~Connect_state(){}
};
class Connect_close:public Connect_state
{
private:
    Connect_close(){}
public:
    Connect_close(const Connect_close& obj) = delete;
    Connect_close& operator=(const Connect_close& obj) = delete;
    static Connect_close*getInstance()
    {
        static Connect_close m_state;
        return &m_state;
    }
    void operation1()
    {
        std::cout<<"连接断开..."<<endl;
    }
};
class Connect_link:public Connect_state
{
private:
    Connect_link(){}
public:
    Connect_link(const Connect_link& obj) = delete;
    Connect_link& operator=(const Connect_link& obj) = delete;
    static Connect_link*getInstance()
    {
        static Connect_link m_state2;
        return &m_state2;
    }
    void operation1()
    {
        std::cout<<"连接成功..."<<endl;
        Connect_close::getInstance()->operation1();
    }
};
int main()
{
    Connect_state *cs=Connect_link::getInstance();
    cs->operation1();
    return 0;
}


