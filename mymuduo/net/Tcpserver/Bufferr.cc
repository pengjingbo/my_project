#include "Buffer.h"

#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

// 利用readv可以实现多个缓冲区的读取数据
ssize_t Buffer::readFd(int fd, int *savedErrno)
{
    char extrabuf[65536];
    struct iovec iov[2];
    const size_t writeable = writableBytes();
    iov[0].iov_base = beginWrite();
    iov[0].iov_len = writeable;
    iov[1].iov_base = extrabuf;
    iov[1].iov_len = sizeof extrabuf;

    const int iovcnt = (writeable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = ::readv(fd, iov, iovcnt);
    if (n < 0)
    {
        *savedErrno = errno;
    }
    else if (static_cast<size_t>(n) <= writeable)
    {
        writerIndex_ += n;
    }
    else
    { // 当extrabuf和buffer_中都有数据时，需要把extrabuf中的数据append到buffer_上
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writeable);
    }
    return n;
}
ssize_t Buffer::writeFd(int fd)
{
    ssize_t n=::write(fd,peek(),readableBytes());
    return n;
}
/* #include<iostream>
int main()
{
    Buffer buffer;
    char buf[100]="Hello World~";
    buffer.append(&buf,sizeof buf);
    std::cout<<buffer.retrieveAllAsString(sizeof buf)<<std::endl;
} */
