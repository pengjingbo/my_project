#include<iostream>
using namespace std;
class Lazy_mode
{
private:
    Lazy_mode(){};
    Lazy_mode(Lazy_mode & t){};  //关闭拷贝构造
    Lazy_mode(Lazy_mode && t){}; //关闭移动构造
    Lazy_mode &operator=(const Lazy_mode &)=default;//关闭赋值运算符'='重载函数。
    int num=0;

public:
    static Lazy_mode*getInstance() //静态变量构造单个实例，没有线程安全等问题
    {
        static Lazy_mode t;
        return &t;
    }
    void test(int t)
    {
        cout<<num<<endl;
        num=t;
    }
};
int main()
{
    Lazy_mode* lm=Lazy_mode::getInstance();
    lm->test(100);
    Lazy_mode::getInstance()->test(90);
    return 0;
}
