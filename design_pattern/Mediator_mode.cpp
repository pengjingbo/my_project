#include<iostream>
#include<memory>
#include<string>
#include<unordered_set>
using namespace std;
/*  中介者模式，中介者模式是一种行为设计模式， 能让你减少对象之间混乱无序的依赖关系。 
该模式会限制对象之间的直接交互， 迫使它们通过一个中介者对象进行合作。 
个人理解，就像计算机网络中的路由器组成的子网一样，每台设备通过路由器进行相互之间的通信，路由器就相当于中介者
当每台设备都想要向对方相互发送信息时，就会导致对象之间依赖关系的紧耦合，当会限制对象之间的直接交互， 
迫使它们通过一个中介者对象进行合作。 */
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
class Mediator;
class Colleague {
public:
    virtual void send(std::string message) = 0;
    virtual void receive(std::string message) = 0;
    virtual void setMediator(std::shared_ptr<Mediator> mediator) = 0;
    virtual const string & getName()=0;
};

class Mediator {
public:
    virtual void addColleague(std::shared_ptr<Colleague> colleague) = 0;
    virtual void send(std::string name, std::string message) = 0;
};

class ConcreteMediator : public Mediator {
public:
    void addColleague(std::shared_ptr<Colleague> colleague) override {
        _colleagues.push_back(colleague);
    }

    void send(std::string name, std::string message) override {
        for (auto& colleague : _colleagues) {
            if (colleague->getName() != name) {
                colleague->receive(message);
            }
        }
    }

private:
    std::vector<std::shared_ptr<Colleague>> _colleagues;
};

class ConcreteColleague : public Colleague {
public:
    ConcreteColleague(std::string name) : _name(name) { }

    void send(std::string message) override {
        _mediator->send(_name, message);
    }

    void receive(std::string message) override {
        std::cout << _name << " received message: " << message << std::endl;
    }

    void setMediator(std::shared_ptr<Mediator> mediator) override {
        _mediator = mediator;
    }
    const string& getName() override
    {
        return this->_name;
    }
private:
    std::string _name;
    std::shared_ptr<Mediator> _mediator;
};


int main() {
    std::shared_ptr<Mediator> mediator = std::make_shared<ConcreteMediator>();

    std::shared_ptr<Colleague> colleague1 = std::make_shared<ConcreteColleague>("Alice");
    std::shared_ptr<Colleague> colleague2 = std::make_shared<ConcreteColleague>("Bob");
    std::shared_ptr<Colleague> colleague3 = std::make_shared<ConcreteColleague>("Charlie");

    mediator->addColleague(colleague1);
    mediator->addColleague(colleague2);
    mediator->addColleague(colleague3);

    colleague1->setMediator(mediator);
    colleague2->setMediator(mediator);
    colleague3->setMediator(mediator);

    colleague1->send("Hello everyone!");

    return 0;
}