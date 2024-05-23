#include<iostream>
#include<string>
#include<vector>
using namespace std;
//命令模式，根本目的是实现“行为请求者”和“行为实现这”的解耦，常用的实现方法为“将行为抽象为对象”
//此外该模式和c++中的函数对象很类似，c++中的函数对象可以通过模板实现编译过程的多态，命令模式则是在运行时实现
class Command
{

public:
    virtual  void addCommand(Command *c){}
    virtual void execute()=0;
    virtual ~Command(){}
};
class Command1 :public Command
{
public:
    void execute()
    {
        cout<<"执行命令Command1\n";
    }
};
class Command2 :public Command
{
public:
   void execute()
    {
        cout<<"执行命令Command2\n";
    }
};
class Marco_Command :public Command
{
private:
    std::vector<Command *>m_set;
public:
    void addCommand(Command *c)
    {
        m_set.push_back(c);
    }
    void execute()
    {
        for(auto i:m_set) i->execute();
    }
};
int main()
{
    Command *c1=new Command1;
    Command *c2=new Command2;

    Command *marco_c=new Marco_Command;
    marco_c->addCommand(c1);
    marco_c->addCommand(c2);
    marco_c->execute();
}


