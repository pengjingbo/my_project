#include<iostream>
#include<string>
using namespace std;
//代理模式，其实就是特殊的门面模式(子系统中调用的对象只有一个，而且接口要继承对象类，)
//即在访问一个对象的控制接口时，需要在访问对象时执行一些额外操作（例如，延迟加载、缓存、日志记录等）时，可以使用代理模式。
class Subject {
public:
    virtual ~Subject() {}
    virtual void request() const = 0;
};

// 真实主题
class RealSubject : public Subject {
public:
    void request() const override {
        std::cout << "RealSubject: handling request" << std::endl;
    }
};

// 代理
class Proxy : public Subject {
public:
    Proxy() : realSubject_(new RealSubject()) {}

    ~Proxy() {
        delete realSubject_;
    }

    void request() const override { //在访问接口的时候要附加一起其他的操作
        std::cout << "Proxy: performing some actions before the request" << std::endl;
        realSubject_->request();
        std::cout << "Proxy: performing some actions after the request" << std::endl;
    }

private:
    RealSubject* realSubject_;
};

int main() {
    Proxy proxy;
    proxy.request();
    return 0;
}