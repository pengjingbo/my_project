#include<iostream>
using namespace std;
class Lazy_mode
{
private:
    Lazy_mode(){};
    Lazy_mode(Lazy_mode & t){};  //关闭拷贝构造
    Lazy_mode(Lazy_mode && t){}; //关闭移动构造
    Lazy_mode &operator=(const Lazy_mode &)=default;//关闭赋值运算符'='重载函数。
    static Lazy_mode *lm;
    int num=0;

public:
    static Lazy_mode*getInstance() //只有静态成员函数才能调用静态成员变量
    {
        return lm;
    }
    void test(int t)
    {
        cout<<num<<endl;
        num=t;
    }
};
Lazy_mode * Lazy_mode::lm=new Lazy_mode;

int main()
{
    Lazy_mode* lm=Lazy_mode::getInstance();
    lm->test(100);
    Lazy_mode::getInstance()->test(90);
    return 0;
}
