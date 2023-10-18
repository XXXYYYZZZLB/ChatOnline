#include "include/Channel.h"
#include <sys/epoll.h>
#include <iostream>

Channel::Channel() {}

Channel::Channel(int fd) : fd_(fd) {}

Channel::~Channel() {}

int Channel::getFd() { return fd_; }
void Channel::setFd(int fd) { fd_ = fd; }

void Channel::setRevents(__uint32_t ev) { revents_ = ev; }
void Channel::setEvents(__uint32_t ev) { events_ = ev; }

__uint32_t Channel::getEvents() { return events_; }
__uint32_t Channel::getRevents() { return revents_; }

void Channel::setReadHandler(CallBack &&func) { readHandler_ = func; }
void Channel::setWriteHandler(CallBack &&func) { writeHandler_ = func; }
void Channel::setErrorHandler(CallBack &&func) { errorHandler_ = func; }

/*
    POLLIN ：  表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
    EPOLLOUT： 表示对应的文件描述符可以写；
    EPOLLPRI： 表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
    EPOLLERR： 表示对应的文件描述符发生错误；
    EPOLLHUP： 表示对应的文件描述符被挂断（EPOLLRDHUP）
    EPOLLET：  将 EPOLL设为边缘触发
*/
void Channel::handleEvents()
{ // 根据监听到是事件执行回调函数
    events_ = 0;
    if (revents_ & EPOLLERR)
    {
        handleError();
        return;
    }
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
    {
        handleRead();
    }
    if (revents_ & EPOLLOUT)
    {
        handleWrite();
    }
}

void Channel::handleRead()
{
    if (readHandler_ != nullptr)
        readHandler_();
}
void Channel::handleWrite()
{
    if (writeHandler_ != nullptr)
        writeHandler_();
}
void Channel::handleError()
{
    if (errorHandler_ != nullptr)
        errorHandler_();
}