#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <vector>
#include <string.h>
#include <string>

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0 kCheapPrepend =8  <=      readerIndex   <=   writerIndex    <=     size

// TCP连接时用户态的缓冲区（从内核中的缓冲区读取数据后存放的地方）
class Buffer
{
public:
    static const size_t kCheapPrepend = 8;  // 可以记录数据包的长度，
    static const size_t kInitalSize = 1024; // buffer初始化的默认大小
    
    explicit Buffer(size_t initalSize=kInitalSize)
                    : buffer_(kCheapPrepend+initalSize)
                    ,readerIndex_(kCheapPrepend)
                    ,writerIndex_(kCheapPrepend)
    {
    }

    size_t readableBytes() const
    {
        return writerIndex_ - readerIndex_;
    }
    size_t writableBytes() const
    {
        return buffer_.size() - writerIndex_;
    }
    size_t prependableBytes()
    {
        return readerIndex_;
    }

    // 返回指向可读区域的头指针
    const char *peek() const
    {
        return begin() + readerIndex_;
    }
    // 可读区域的数据被全部取走，读写指针都全部回到起点
    void retrieveAll()
    {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }
    // 取走可读数据后移动读指针
    void retrieve(size_t len)
    {
        if (len < readableBytes())
        {
            readerIndex_ += len;
        }
        else
        {
            retrieveAll();
        }
    }
    // 取走数据并转化成string类型
    std::string retrieveAsString(size_t len)
    {
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }
    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    
    // 确认写区域的大小是否足够,不够的话需要调用makeSpace()进行扩容
    void ensureWritableBytes(size_t len)
    {
        if (writableBytes() < len)
        {
            makeSpace(len);
        }
    }
    // 写入数据
    void append(const char *data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        writerIndex_ += len;
    }
    void append(const void *data, size_t len)
    {
        append(static_cast<const char*>(data),len);
    }
    // 返回写指针所在位置的指针
    char *beginWrite()
    {
        return begin() + writerIndex_;
    }
    const char *beginWrite() const
    {
        return begin() + writerIndex_;
    }

    ssize_t readFd(int fd,int* savedErrno);
    ssize_t writeFd(int fd);
private:
    // 返回底层容器vector的首元素地址
    char *begin()
    {
        //*iterator表示迭代所指向元素本身，&*iterator表示表示迭代所指向元素的地址
        return &*buffer_.begin();
    }
    const char *begin() const
    {
        return &*buffer_.begin();
    }
    // 扩容
    void makeSpace(size_t len)
    {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            buffer_.resize(writerIndex_ + len);
        }
        else
        {
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_
            , begin() + writerIndex_
            , begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
        }
    }

private:
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};

#endif