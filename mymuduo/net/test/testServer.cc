#include<mymuduo/TcpServer.h>
#include<mymuduo/TcpConnection.h>
#include<string>
using namespace net;
using namespace std::placeholders;
class EchoServer
{
public:
    EchoServer(EventLoop *loop, const InetAddress &listenAddr, const std::string nameArg)
                :server_(loop,listenAddr,nameArg),mainloop_(loop)
                {
                    server_.setThreadNum(3);
                    server_.setMessageCallback(std::bind(&EchoServer::OnmessageCallback,this,_1,_2,_3));
                }
    void start()
    {
        server_.start();
    }            
    
private:
    void OnmessageCallback(const TcpConnectionPtr& ptr,Buffer *buf,Timestamp time)
    {
        std::string msg=buf->retrieveAllAsString();
        ptr->send(msg);
        //ptr->shutdown();
    }

    TcpServer server_;
    EventLoop *mainloop_;
};

int main()
{
    EventLoop loop_;
    InetAddress addr(6666);
    std::cout<<addr.toIpPort()<<std::endl;
    EchoServer Server_(&loop_,addr,"EchoServer01");
    Server_.start();
    loop_.loop();

}

