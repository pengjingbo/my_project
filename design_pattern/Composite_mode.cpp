#include<iostream>
#include<list>
#include<memory>
#include<unordered_set>
using namespace std;
/* 你可以使用它将对象组合成树状结构， 并且能像使用独立对象一样使用它们。如果应用的核心模型能用树状结构表示， 在应用中使用组合模式才有价值。
实现方法:
1.确保应用的核心模型能够以树状结构表示。 尝试将其分解为简单元素和容器。
 记住， 容器必须能够同时包含简单元素和其他容器。

2.声明组件接口及其一系列方法， 这些方法对简单和复杂元素都有意义。

3.创建一个叶节点类表示简单元素。 程序中可以有多个不同的叶节点类。

4.创建一个容器类表示复杂元素。 在该类中， 创建一个数组成员变量来存储对于其子元素的引用。
 该数组必须能够同时保存叶节点和容器， 因此请确保将其声明为组合接口类型。

5.实现组件接口方法时， 记住容器应该将大部分工作交给其子元素来完成。

6.最后， 在容器中定义添加和删除子元素的方法。
记住， 这些操作可在组件接口中声明。 这将会违反接口隔离原则， 因为叶节点类中的这些方法为空。
 但是， 这可以让客户端无差别地访问所有元素， 即使是组成树状结构的元素。 */

class Component
{
public:
    virtual int operation()=0;
    virtual void add(std::shared_ptr<Component>obj)=0;
    virtual void remove(std::shared_ptr<Component>obj)=0;
    virtual~Component(){}
};

class leaf:public Component
{    
public:
    leaf(int price_):m_price(price_){}
	virtual void add(std::shared_ptr<Component>obj){}
    virtual void remove(std::shared_ptr<Component>obj){}
    int operation() override
    {
        return m_price;
    }
private:
    int m_price;
};

class Composite:public Component
{    
public:
    void add(std::shared_ptr<Component>obj) override
    {
        m_list.push_back(obj);
    }
    void remove(std::shared_ptr<Component>obj) override
    {
        m_list.remove(obj);
    }
    int operation() override
    {
        int total=0;
        for(auto i:m_list)
        {
            total+=i->operation();
        }
        return total;
    }
private:
    std::list<std::shared_ptr<Component>>m_list;
};
int main()
{
    std::shared_ptr<Composite> root = std::make_shared<Composite>();
    root->add(std::make_shared<leaf>(11));
	root->add(std::make_shared<leaf>(12));
	root->add(std::make_shared<leaf>(13));
    std::shared_ptr<Composite> comp1 = std::make_shared<Composite>();
    comp1->add(std::make_shared<leaf>(11));
	comp1->add(std::make_shared<leaf>(12));
	comp1->add(std::make_shared<leaf>(13));
    root->add(comp1);
    cout<<root->operation();
}

