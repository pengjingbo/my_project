#include<iostream>
#include<string>
using namespace std;
//工厂模式首先要写生产的产品，产品本身也是由一个产品基类衍生出来的
class Car
{
public:
	Car(string name) :_name(name) {}
	virtual void show() = 0;
protected://让子类可以使用_name
	string _name;
};
class Bmw:public Car
{
public:
	Bmw(string name) :Car(name) {}
	void show() override
	{
		cout << "获取了一辆宝马汽车：" << _name << endl;
	}
};
class Aodi:public Car
{
public:
	Aodi(string name) :Car(name) {}
	void show() override
	{
		cout << "获取了一辆宝马汽车：" << _name << endl;
	}
};
//
/////
//每一个工厂的虚函数参数列表都应该相同
class Basic_Factory
{
public:
    virtual Car* Factory_create(string name)=0;
};

class BWM_factory :public Basic_Factory
{
public:
    Car* Factory_create(string name) override
    {
       return new Bmw(name);
    }

};

class Aodi_factory :public Basic_Factory
{
public:
    Car* Factory_create(string name) override
    {
       return new Aodi(name);
    }

};
//main()函数中选择一个具体的工厂
int main()
{
    Basic_Factory *bwmfc=new BWM_factory;
    Car*bwm=bwmfc->Factory_create("x6");
    bwm->show();
    return 0;
}


