#include<iostream>
#include<string>
using namespace std;
//模板方法，关键就是在与由基类创建一个流程的框架，再由子类重写纯虚函数和虚函数来变化流程
//在这个模式中，首先要确定稳定的框架，在确定哪些部分是可以通过晚绑定来重写的
class Library
{
public:
    virtual void step1()=0;
    virtual void step2()=0;
    void step3()
    {
        cout<<"付款成功！"<<endl;
    };

    void run()
    {
        step1();
        step2();
        step3();
    };
    virtual ~Library(){}
};
class MeiTuan :public Library
{
public:
    void step1() override
    {
    cout<<"打开美团外卖的app"<<endl;
    };
    void step2() override
    {
    cout<<"选择吃牛肉"<<endl;
    };
};
class ELeMe :public Library
{
public:
    void step1() override
    {
    cout<<"打开饿了么外卖的app"<<endl;
    };
    void step2() override
    {
    cout<<"选择吃牛肉"<<endl;
    };
};
int main()
{
    Library *lb=new ELeMe;
    lb->run();
    delete lb;
    Library *lb2=new MeiTuan;
    lb2->run();
    delete lb2;
}