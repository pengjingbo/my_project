#include<iostream>
using namespace std;
//策略模式，就是定义一系列的算法，使得它们能够相互替换，独立于客户端而变化
//也就是说他们的它们是相同，对外的接口也是相同的，只是不同的策略使得环境角色表现出不同的行为
class AbstractStrategy
{
public:
    virtual void AlgorithmStrategy()=0;
    virtual ~AbstractStrategy(){}
};
class StrategyA: public AbstractStrategy
{
public:
    void AlgorithmStrategy()override
    {
        cout<<"算法A的实现"<<endl;
    }
};
class StrategyB: public AbstractStrategy
{
public:
    void AlgorithmStrategy()override
    {
        cout<<"算法B的实现"<<endl;
    }
};
class StrategyC: public AbstractStrategy
{
public:
    void AlgorithmStrategy()override
    {
        cout<<"算法C的实现"<<endl;
    }
};
//调用策略的端口（可以配合简单工厂模式使用）
class context
{
public:
    context(AbstractStrategy * sty):m_strategy_(sty){};
    void  run()
    {
        m_strategy_->AlgorithmStrategy();
    }
    ~context()
    {
        delete m_strategy_;
    };
private:
    AbstractStrategy * m_strategy_;    
};

int main()
{
    context c(new StrategyA);
    c.run();
    return 0;
}


