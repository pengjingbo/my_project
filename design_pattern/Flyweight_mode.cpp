#include<iostream>
#include<string>
#include<unordered_map>
#include <memory>
using namespace std;
/* 简单的理解: 一个类的成员非常多，创建此对象很消耗资源，在实际场景中又需要反复创建和销毁该对象。所消耗的内存，就更加庞大。

如果此时设计一个对象池，里面缓存一定的对象，软件在用时申请，不用时回收。就能实现对象的重复利用，而多次创建和销毁对象。 */
/* 个人理解：每当客户端请求一个享元对象时，工厂会检查是否已经存在具有相同内部状态的对象。
    如果存在，则返回已有的对象；如果不存在，则创建新的具体享元对象并存储起来供后续请求使用。
    这样，多个带有不同外部状态的对象就可以共享同一个具有固定内部状态的具体享元对象，从而节约内存。 */

//享元对象
class Website
{
protected:
    string m_string;        //内部状态
public:
    Website(const string&  name):m_string(name){}
    virtual void use(const string& user)=0;
    virtual ~Website(){}
};

class ConcreteWebsite :public Website
{
public:
    ConcreteWebsite(const string& name):Website(name){}
    void use(const string& user) override       //使用享元对象的内部状态和外部状态进行操作                      
    {
        cout<<user<<"使用网站:"<<m_string<<endl;
    }
};

class Website_factory
{
private:
    unordered_map<string,std::shared_ptr<ConcreteWebsite>> m_map;
public:
    std::shared_ptr<ConcreteWebsite> return_website(const string& name)//返回具有相同内部状态的实例
    {
        if(m_map.find(name)==m_map.end())
        {
            m_map[name]=make_shared<ConcreteWebsite>("www."+name+".com");
        }
        return m_map[name];
    }
};
int main()
{
    Website_factory *fc=new Website_factory;
    shared_ptr<ConcreteWebsite> ptr1=fc->return_website("博客");
    ptr1->use("小王");
    shared_ptr<ConcreteWebsite> ptr2=fc->return_website("博客");
    ptr2->use("小王");
    shared_ptr<ConcreteWebsite> ptr3=fc->return_website("baidu");
    ptr3->use("小王");
}


