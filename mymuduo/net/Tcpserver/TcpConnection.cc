#include"TcpConnection.h"
#include"Socket.h"
#include "EventLoop.h"
#include"Channel.h"
#include"Logger.h"

#include<errno.h>

using namespace std::placeholders;

void net::defaultConnectionCallback(const TcpConnectionPtr &conn)
{
    if(conn->connected())
    {
    LOG_INFO("%s -> %s is UP\n",conn->localAddress().toIpPort().c_str()
                              ,conn->peerAddress().toIpPort().c_str());
    }
    else
    {
     LOG_INFO("%s -> %s is DOWN\n",conn->localAddress().toIpPort().c_str()
                              ,conn->peerAddress().toIpPort().c_str());   
    }

}
void net::defaultMessageCallback(const TcpConnectionPtr &, Buffer *buffer, Timestamp receiveTime)
{
    buffer->retrieveAll();
}

static EventLoop *CheckLoopNotNull(EventLoop *loop)
{
    if (loop == nullptr)
    {
        LOG_ERROR("%s:%s:%d Tcpconnection loop is null!\n", __FILE__, __FUNCTION__, __LINE__);
    }
    return loop;
}

TcpConnection::TcpConnection(EventLoop *loop
                , const std::string &nameArg
                , int sockfd
                , const InetAddress &localAddr
                , const InetAddress &peerAddr)
                : loop_(CheckLoopNotNull(loop))
                , name_(nameArg)
                , state_(kConnecting)
                , reading_(true)
                , socket_(new Socket(sockfd))
                , channel_(new Channel(loop,sockfd))
                , localAddr_(localAddr)
                , peerAddr_(peerAddr)
                , highWaterMark_(64*1024*1024)//64M,当待发送的数据大小超过这个值的时候就会触发回调
{
    channel_->setReadCallback(
        std::bind(&TcpConnection::handleRead, this, _1));
    channel_->setWriteCallback(
        std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(
        std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(
        std::bind(&TcpConnection::handleError, this));

        LOG_INFO("TcpConnection::ctor[%s] at fd=%d state=%d\n"
        ,name_.c_str(),socket_->fd(),(int)state_);
        socket_->setKeepAlive(true);
}
TcpConnection::~TcpConnection()
{
    LOG_INFO("TcpConnection::dtor[%s] at fd=%d state=%d\n"
        ,name_.c_str(),socket_->fd(),(int)state_);
}

//当channel上有读事件发生的时候执行这个回调来处理
void TcpConnection::handleRead(Timestamp receiveTime)
{
    int savederrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &savederrno);
    if (n > 0)
    {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    }
    else if (n == 0) // 当对端关闭连接时，返回值为0，调用handleClose()
    {
        handleClose();
    }
    else
    { // 当出现错误时，返回值小于0，调用handleError()
        errno = savederrno;
        LOG_ERROR("TcpConnection::handleRead\n");
        handleError();
    }
}
//当channel上有写事件发生的时候执行这个回调来处理
void TcpConnection::handleWrite()
{
    ssize_t n = outputBuffer_.writeFd(socket_->fd());
    if (channel_->isWriting())//channel可写
    {
        if (n > 0)
        {
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0)
            {
                //当outputBuffer_中没有数据了就关闭channel_的写事件
                channel_->disableWriting();
                //如果用户设置了writeCompleteCallback_的回调函数，就立即执行该回调
                if (writeCompleteCallback_)
                {
                    /* 
                    *首先loop_所在的线程一定和调用loop_的subreactor一样，且这个时候poller在执行写回调
                    *一定不会阻塞在pool(),所以这里调用queueInLoop()的目的就是，把writeCompleteCallback_
                    *添加到loop_将要执行的pendingFunctors_中，一定再一次唤醒loop_来执行writeCompleteCallback_ 
                    */
                    loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                }
                //如果这个时候连接状态发生改变
                if (state_ == kDisconnecting)
                {
                    shutdownInLoop();
                }
            }
        }
        else
        {
            LOG_ERROR("TcpConnection::handleWrite\n");
        }
    }
    else//channel不可写
    {
        LOG_INFO("Connection fd =%d is down, no more writing\n", socket_->fd());
    }
}

//当channel上的事件为关闭事件时执行的回调
void TcpConnection::handleClose()
{
    LOG_INFO("fd = %d  state = %d\n",channel_->fd(),(int)state_);
    setState(kDisconnected);
    channel_->disableAll();

    TcpConnectionPtr connptr(shared_from_this());

    connectionCallback_(connptr);//connectionCallback_=>TcpServer::connectionCallback_
    closeCallback_(connptr);     //closeCallback_ => TcpServer::removeConnection
}
//当channel上的事件为错误事件时执行的回调(也就是connfd上发生了错误)
void TcpConnection::handleError()
{
    int optval, err = 0;
    socklen_t optlen = sizeof optval;
    // 获取connfd上发生的错误
    if (::getsockopt(channel_->fd(), SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
    {
        err = errno;
    }
    else
    {
        err = optval;
    }
    LOG_ERROR("TcpConnection::handleError name=%s - SO_ERROR=%d\n", name_.c_str(), err);
}

//由用户调用的可以给对端发送数据的函数
void TcpConnection::send(const std::string &message)
{
    if (state_ == kConnected) // 和对端的连接状态为:已连接
    {
        
        if (loop_->isInLoopThread())
        {
            sendInLoop(message.c_str(), message.size());
        }
        else
        {
            //向EventLoop注册sendInLoop该回调函数
            loop_->queueInLoop(std::bind(&TcpConnection::sendInLoop
                            , this
                            , message.c_str()
                            , message.size()));
        }
    }
}

//发送数据时，如果内核发送数据慢，用户写入的数据快，就需要把未发送完的数据通过outputBuffer_保存，等待下一次epollout事件触发
void TcpConnection::sendInLoop(const void *data, size_t len)
{
    ssize_t nwrote = 0;     // 已写的数据长度
    size_t remaining = len; // 剩余的数据长度

    bool faultError = false;
    if (state_ == kDisconnected) // 连接断开的话
    {
        LOG_WARN("disconnected, give up writing\n");
        return;
    }

    // 表示channel还未注册写事件并且输出缓冲区中没有数据，当输出缓冲区有数据时不会出现channel还未注册写事件的这种情况
    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
    {
        nwrote = ::write(socket_->fd(), data, len);
        if (nwrote >= 0)
        {
            remaining = len - nwrote;
            if (remaining == 0 && writeCompleteCallback_)
            {
                loop_->queueInLoop(std::bind(writeCompleteCallback_
                                    ,shared_from_this()));
            }
        }
        else // nwrote<0
        {
            nwrote = 0;
            if (errno != EWOULDBLOCK) // 非阻塞IO中，数据还未准备好
            {
                LOG_ERROR("TcpConnection::sendInLoop\n");
                // EPIPE:当服务器的在向对端写数据的时候，客户端的读端关闭了
                // ECONNRESET:当服务器向对端写数据的时候，客户端直接断开连接了或者重启连接了，但还未建立连接
                if (errno == EPIPE || errno == ECONNRESET)
                {
                    faultError = true;
                }
            }
        }
    }

    /*
    *当向对端写数据但数据还未写完时，就需要判断(输出缓冲区的数据+升序数据长度)和高水位标志的大小，
    *当前者大于后者时就需要执行高水位回调函数，然后再把升序的数据追加到输出缓冲区中，并向pollerr注册channel的Epollout事件，
    *等待下一次事件触发WriteCallback_,也就是执行TcpConnection::handlewrite()再把剩余的数据从输出缓冲区中发送出去
    */
    if (!faultError && remaining > 0)
    {
        size_t oldLen=outputBuffer_.readableBytes();

        //oldLen<highWaterMark_，保证输出缓冲区中原来的数据大小是不足以触发highWaterMarkCallback_的
        if(oldLen+remaining>=highWaterMark_
            &&oldLen<highWaterMark_
            &&highWaterMarkCallback_)
        {
            loop_->queueInLoop(std::bind(highWaterMarkCallback_
                                ,shared_from_this()
                                ,oldLen+remaining));
        }
        //把剩余的数据添加到输出缓冲区中
        outputBuffer_.append(static_cast<const char *>(data)+ nwrote, remaining);
        if (!channel_->isWriting())
        {
            //注册channel的写事件
            channel_->enableWriting();
        }
    }
}

//关闭和connfd之间的连接
void TcpConnection::shutdown()
{
    if(state_==kConnected)
    {
        setState(kDisconnecting);//修改连接的状态
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop,shared_from_this()));
    }
}

void TcpConnection::shutdownInLoop()
{
    //channel目前没有监听写connfd事件的话，就关闭channel的写端
    if(!channel_->isWriting())
    {
        socket_->shutdownWrite();
    }
}

//TcpServer::newConnection => connectEstablished 
void TcpConnection::connectEstablished()
{
    setState(kConnected);
    channel_->tie(shared_from_this());//将channel中的weak_ptr绑定当前的Tcpconnection
    channel_->enableReading();        //设置channel状态为可读

    //TcpServer::newConnection => setConnectionCallback
    connectionCallback_(shared_from_this());
}

//将该TcpConnection管理的channel对象从pooler中移除。
void TcpConnection::connectDestroyed()
{
    if(state_==kConnected)
    {
        setState(kDisconnected);
        channel_->disableAll();                 //关闭channel注册过的所有事件
        connectionCallback_(shared_from_this());//执行用户设置的连接断开的回调函数
    }
    channel_->remove();                         //从poller上移除当前的channel；
}