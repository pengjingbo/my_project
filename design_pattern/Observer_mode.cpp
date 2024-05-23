#include<iostream>
#include <unordered_set>
using namespace std;
//观察者模式的核心就是抽象的依赖通知关系，具体的来说，当被观察者发生变化时，通知所有的观察者，但所有观察者接下来的具体的变化都与被观察者无关
//被观察者可以是某个具体的类，只要它的类中包含了处理依赖通知关系的特性就可以作为一个被观察者
//每一个观察者都必须要继承观察者的基类，只有这样，被观察者才能通过观察者基类的指针通知每一个观察者
//观察者
class Observer
{
public:
    virtual void update(int num)=0;
    virtual ~Observer(){};
};

class ob1 :public Observer
{
public:
    void update(int num) override
    {
        cout<<"ob1类接收到了被观察者的状态更新,被观察者的状态为:"<<num<<endl;
    };
};
class ob2 :public Observer
{
public:
    void update(int num)  override
    {
        cout<<"ob2类接收到了被观察者的状态更新,被观察者的状态为:"<<num<<endl;
    };
};
class ob3 :public Observer
{
public:
    void update(int num) override
    {
        cout<<"ob3类接收到了被观察者的状态更新,被观察者的状态为:"<<num<<endl;
    };
};
//被观察者类
class subject
{
public:
    void  Attach(Observer * ob)
    {
        if(m_set.find(ob)==m_set.end())
        {
            m_set.emplace(std::forward<Observer*>(ob));
        }else{
            cout<<"已经添加再观察者队列中了"<<endl;
        }
        
    }
    void  Detach(Observer * ob)
    {
        if(m_set.find(ob)!=m_set.end())
        {
            m_set.erase(ob);
        }else{
            cout<<"不在观察者队列中"<<endl;
        }
    }
    void notify(const int& num)
    {
        for (auto i:m_set) {
            i->update(num);
        }
    }
private:
    unordered_set<Observer *>m_set;
};
int main()
{
    subject sj;
    Observer *ob1_=new ob1;
    Observer *ob2_=new ob2;
    Observer *ob3_=new ob3;
    sj.Attach(ob1_);
    sj.Attach(ob2_);
    sj.Attach(ob3_);
    sj.notify(199);
    sj.Detach(ob1_);
    sj.Detach(ob1_);
    sj.notify(666);
    delete ob1_;
    delete ob2_;
    delete ob3_;
}


