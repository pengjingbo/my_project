#include<iostream>
#include<string>
using namespace std;
/* 建造者模式的优点：

封装性好。有效地封装了建造过程（主要业务逻辑），使得系统整体的稳定性得到了一定保证。

解耦。产品本身和建造过程解耦，相同的建造过程可以创建出不同的产品。

产品建造过程精细化。该模式注重产品创建的整个过程，将复杂的步骤拆解得到多个相对简单的步骤，使得系统流程更清晰，且对细节的把控更精准。

易于扩展。如果有新产品需求，只需要添加一个建造者类即可，不需要改动之前的代码，符合开闭原则。

建造者模式的缺点：

产品的组成部分和构建过程要一致，限制了产品的多样性。

若产品内部有结构上的变化，则整个系统都要进行大改，增加了后期维护成本。 */
//建造者模式将一个复杂对象的构造与它的表示分离，使同样的构建过程可以创建不同的表示。
//具体的来说，当你想要建造一个房子（房子里有很多的属性，窗户，地板，门，墙壁等，而且这些属性的变化是剧烈的，但将他们组合起来的算法——
//即生成一栋房子是很稳定的），这种时候就可以使用建造者模式，建造者模式由三个抽象类组成：产品类，构造者类，管理者类
//产品：即房子，
//构造者：即生产房子中的属性，然后将其组合起来成一个房子
//管理者：控制着构造者，可以将房子的属性组合起来成一个房子的算法，这个算法是稳定的,即先生成窗，在生成门，在生成墙，最后生成地板等。
//严格来说， 你的程序中并不一定需要管理者类。 客户端代码可直接以特定顺序调用创建步骤。
//
//
//产品类
class House
{
public:
    string m_door,m_window,m_wall,m_floor;
    //这里的房子的属性最好设置为私有，然后通过公有函数修改，偷个懒
    void show()
    {
        cout<<"这间房子的门是："<<m_door<<
        "窗户是："<<m_window<<"墙壁是："<<m_wall
        << "地板是："<<m_floor;
    }
};
//构造者类
class HouseBuilder
{
protected:
    House*m_house=new House;
public:
    virtual void create_door()=0;
    virtual void create_window()=0;
    virtual void create_wall()=0;
    virtual void create_floor()=0;
    virtual House *get_Product()=0;
    virtual ~HouseBuilder(){
        delete m_house;
    }
};
class StoneBuilder :public HouseBuilder
{
public:
    void create_door()
    {
        m_house->m_door="石头门,";
    }
    void create_window()
    {
        m_house->m_window="石头窗,";
    }
    void create_wall()
    {
        m_house->m_wall="石头墙,";
    }
    void create_floor()
    {
        m_house->m_floor="石头地板,";
    }
    House *get_Product()
    {
       return m_house;
    }
};
class WoodBuilder :public HouseBuilder
{
public:
    void create_door()
    {
        m_house->m_door="木门,";
    }
    void create_window()
    {
        m_house->m_window="木窗,";
    }
    void create_wall()
    {
        m_house->m_wall="木墙,";
    }
    void create_floor()
    {
        m_house->m_floor="木地板,";
    }
    House *get_Product()
    {
       return m_house;
    }
};
//管理者
class Diraector
{
private:
    HouseBuilder *m_builder;
public:
    Diraector(HouseBuilder *builder):m_builder(builder){};
    void construct()//确定建造流程
    {
        m_builder->create_floor();
        m_builder->create_wall();
        m_builder->create_door();
        m_builder->create_window();
        House *house=m_builder->get_Product();
        house->show();
    }
    ~Diraector()
    {
        delete m_builder;
    }
};
int main()
{
    Diraector *dit=new Diraector(new StoneBuilder);
    dit->construct();
    delete dit;
}

