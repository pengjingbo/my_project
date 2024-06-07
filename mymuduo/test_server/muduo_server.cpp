#include <iostream>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Timestamp.h>
using namespace std;
using namespace muduo::net;
using namespace muduo;
using namespace std::placeholders;
class ChatServer
{

public:
    ChatServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const string &nameArg) : m_server(loop, listenAddr, nameArg, TcpServer::kReusePort), m_eventloop(loop)
    {

        m_server.setConnectionCallback(std::bind(&ChatServer::On_Connect, this, std::placeholders::_1));

        m_server.setMessageCallback(std::bind(&ChatServer::On_Message, this, _1, _2, _3));

        m_server.setThreadNum(3);
    }
    void start()
    {
        m_server.start();
    }

private:
    void On_Connect(const TcpConnectionPtr &conn)
    {
        cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << " is " << (conn->connected() ? " UP " : " Down ")
        <<endl;
        if(conn->connected()==false) conn->shutdown();
    }
    void On_Message(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time)
    {
        string msg(buf->retrieveAllAsString());
        cout << "recv data:" << msg << " time: " << time.toString()<<endl;;
        conn->send(buf);
    }

    TcpServer m_server;
    EventLoop *m_eventloop;
};

int main()
{
    muduo::net::EventLoop loop_;
    muduo::net::InetAddress listenAddr("127.0.0.1",6000);
    ChatServer server_(&loop_, listenAddr,"Mini_Server");
    server_.start();
    loop_.loop();
    return 0;
}