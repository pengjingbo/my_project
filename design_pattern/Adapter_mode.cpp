#include<iostream>
#include<string>
#include<deque>
using namespace std;
/* 在于将某些功能与第三方需求接口适配对接，且避免第三方接口与功能代码过多耦合。

在设计初初，不要考虑使用此模式。仅在功能完善，需要实现第三方接口时，没必要迎合第三方的需求对原来的设计大动刀戈，可以尝试使用适配器模式。

多用于想应用某些功能，但是功能类的接口与需求的接口不兼容时，采用适配器模式来解决。主要应用在以下场景：

新旧接口兼容软件版本升级，部分旧接口还在被使用。需要保留旧的接口，增加新接口，使两者兼容。
第三方接口的适配在系统功能稳定的情况下，有第三方新的接口需求需要对接。
统一多个类相同功能的接口例如统一不同类型数据库的访问接口。 */
//比如说c++stl容器中的stack和queue的底层实现都是依赖于deque,即deque作为老的接口，可以通过适配器模式，实现stack和queue中新的需求
template <typename T>
class m_stack
{
private:
    deque<T>dq_;
public:
    void pop()
    {
        if(!dq_.empty())dq_.pop_back();
        else cout<<"pop() error,stack is empty!\n";
    }
    void push(T t)
    {
        dq_.push_back(t);
    }
    T top()
    {
        if(!dq_.empty())return dq_.back();
        else {
            cout<<"\ntop()_error,stack is empty!\n";
            return 0;
        }
    }
};
//queue的实现和stack一样
int main()
{
    m_stack<int> s;
    for(int i=0;i<10;i++)s.push(i);
    for(int i=0;i<10;i++)
    {
        cout<<s.top()<<" ";
        s.pop();
    }
    s.top();
} 

