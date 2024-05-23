#include<iostream>
#include<memory>
using namespace std;
/*
* 关键代码：客户与系统之间加一个外观层，外观层处理系统的调用关系、依赖关系等。
*以下实例以电脑的启动过程为例，客户端只关心电脑开机的、关机的过程，并不需要了解电脑内部子系统的启动过程。
*/

//抽象控件类，提供接口
class Control {
public:
    virtual ~Control() = default;//建议将基类的析构函数定义为虚函数
    virtual void start() = 0;

    virtual void shutdown() = 0;
};

//子控件， 主机
class Host : public Control {
public:
    void start() override {
        std::cout << "Host start" << std::endl;;
    }

    void shutdown() override {
        std::cout << "Host shutdown" << std::endl;;;
    }
};

//子控件， 显示屏
class LCDDisplay : public Control {
public:
    void start() override {
        std::cout << "LCD Display start" << std::endl;;
    }

    void shutdown() override {
        std::cout << "LCD Display shutdonw" << std::endl;;
    }
};

//子控件， 外部设备
class Peripheral : public Control {
public:
    void start() override {
        std::cout << "Peripheral start" << std::endl;;
    }

    void shutdown() override {
        std::cout << "Peripheral shutdown" << std::endl;;
    }
};

class Computer {
public:
    void start() {
        m_host.start();
        m_display.start();
        m_pPeripheral->start();
        std::cout << "Computer start" << std::endl;;
    }

    void shutdown() {
        m_host.shutdown();
        m_display.shutdown();
        m_pPeripheral->shutdown();
        std::cout << "Computer shutdown" << std::endl;;
    }

private:
    //私有，屏蔽子系统
    Host m_host;
    LCDDisplay m_display;
    std::shared_ptr<Peripheral> m_pPeripheral=std::make_shared<Peripheral>();
};

int main() {
    Computer computer;
    computer.start();

    std::cout << "=====Client do something else !====" << std::endl;;

    computer.shutdown();
    return 0;
    //运行结果:
    //Host start
    //LCD Display start
    //Peripheral start
    //Computer start
    //=====Client do something else !====
    //Host shutdown
    //LCD Display shutdonw
    //Peripheral shutdown
    //Computer shutdown
}
