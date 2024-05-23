#include<iostream>
#include<string>
using namespace std;
//引擎
class Engine
{
public:  
    virtual string getEngine()=0;
    virtual ~Engine(){} //虚析构函数使得在删除指向子类对象的基类指针时可以调用子类的析构函数达到释放子类中堆内存的目的，而防止内存泄露的.
};
class V6Engine : public Engine
{
public:      
    string getEngine() override
    {
        return "v6引擎!\n";
    };
    
};
class V4Engine : public Engine
{
public:      
    string getEngine() override
    {
        return "v4引擎!\n";
    };
    
};
//车身材料
class flag
{
public:  
    virtual string getflag()=0;
    virtual ~flag(){}
};
class bwm : public flag
{
    public:  
    string getflag() override
    {
        return "宝马!\n";
    };
    
};
class Aodi : public flag
{
  public:  
    string getflag() override
    {
        return "奥迪!\n";
    };
    
};
//每一辆车都是由:引擎和标志组装而成的
class Car
{
public:     
    Car(Engine *engine,flag *flag)
    {
        engine_=engine;
        flag_=flag;
    }
    void shown()
    {
        cout<<"引擎是："<<engine_->getEngine()<<"车牌是 :"<<flag_->getflag();
    }
private:
    Engine *engine_;
    flag *flag_;
};
//工厂
class AbstractFactory
{
public:
    virtual Car *Car_create()=0;
    virtual ~AbstractFactory(){}
};
class bwm_Factory:public AbstractFactory
{
public:
    Car *Car_create()override
    {
        cout<<"一辆宝马车已建造完毕!"<<endl;
        return new Car(new V6Engine,new bwm);
    };
};
class Aodi_Factory:public AbstractFactory
{
public:
    Car *Car_create() override
    {
        cout<<"一辆奥迪车已建造完毕!"<<endl;
        return new Car(new V6Engine,new Aodi);
    };
};
int main()
{
    AbstractFactory *fc=new bwm_Factory;
    Car *m_car=fc->Car_create();
    m_car->shown();
    return 0;
}