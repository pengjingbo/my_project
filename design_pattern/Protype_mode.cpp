#include<iostream>
#include<string>
using namespace std;
//用原型实例指定创建对象的种类，并通过拷贝这些原型创建新的对象，简单理解就是“克隆指定对象”
//客户端使用的时clone出来的对象，原对象是不做任何改变的
//原型模式其实就是一种特殊的工厂方法，它把工厂的职能通过clone函数，保持在了产品里
class Car
{
public:
    virtual void shown()=0;
    virtual ~Car(){}
    virtual Car*clone()=0;
};
class bwm : public Car
{
private:
    string car_name;
    string car_engine;
public:
    bwm(string name,string engine):car_name(name),car_engine(engine){}
    bwm (const bwm &car):car_name(car.car_name),car_engine(car.car_engine){}
    void shown() override
    {
        cout<<"这辆宝马车的名字是"<<car_name<<"搭载的是"<<car_engine<<"引擎\n";
    }
    Car*clone()
    {
        return new bwm(*this );
    }
};
class Aodi : public Car
{
private:
    string car_name;
    string car_engine;
public:
    Aodi(string name,string engine):car_name(name),car_engine(engine){}
    Aodi (const Aodi &car):car_name(car.car_name),car_engine(car.car_engine){}
    void shown() override
    {
        cout<<"这辆宝马车的名字是:"<<car_name<<",搭载的是:"<<car_engine<<"引擎.\n";
    }
    Car*clone()
    {
        return new Aodi(*this );
    }
};

//客户端
class client
{
private:
    Car *m_car;
public :
    client(Car* Car):m_car(Car){}
    void shown()
    {

        Car *copy=m_car->clone();
        copy->shown();
        delete copy;
    }
};
int main()
{
    client *cl=new client(new Aodi("A6","v4"));
    cl->shown();
    delete cl;
}


