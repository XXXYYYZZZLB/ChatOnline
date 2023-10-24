#include "HandleData.h"
#include "util/Util.h"
#include <iostream>

const __uint32_t DEFAULT_EVENT = EPOLLIN | EPOLLET | EPOLLONESHOT;

HandleData::HandleData(EventLoop *loop, int connfd)
    : loop_(loop),
      channel_(new Channel(loop, connfd)),
      fd_(connfd),
      error_(false)
{
    channel_->setReadHandler(std::bind(&HandleData::handleRead, this));
    channel_->setWriteHandler(std::bind(&HandleData::handleWrite, this));
    // channel_->setConnHandler(std::bind(&HttpData::handleConn, this));
}
HandleData::~HandleData()
{
    // close(fd);
}

SP_Channel HandleData::getChannel()
{
    return channel_;
}
EventLoop *HandleData::getLoop()
{
    return loop_;
}

void HandleData::newEvent()
{
    channel_->setEvents(DEFAULT_EVENT);
    loop_->addToPoller(channel_);
}

void HandleData::handleRead()
{
    std::cout << "HandleData::handleRead()--触发可读" << std::endl;
    bool zero = false;
    int read_num = readn(fd_, inBuffer_, zero);
    // zero用于标志是否读取到了数据，true没读到

    std::cout << "read_num: " << read_num << " zero=" << zero << std::endl;

    if (read_num < 0)
    {
        perror("1");
        error_ = true;
        std::cout << "error!" << std::endl;
        return;
    }
    else if (zero)
    { // 没读到数据就关闭连接
        // 有请求出现但是读不到数据，可能是Request
        // Aborted，或者来自网络的数据没有达到等原因
        // 最可能是对端已经关闭了，统一按照对端已经关闭处理
        // error_ = true;
        if (read_num == 0)
        {
            std::cout << "error!" << std::endl;
            return;
        }
    }

    std::cout << "Request: " << inBuffer_ << std::endl;
    bool flag = analysisRequest();
    if (flag)
    {
        std::cout << "analysisRequest 成功" << std::endl;
    }
    else
    {
        std::cout << "analysisRequest 失败" << std::endl;
    }
    handleWrite();
}

bool HandleData::analysisRequest()
{
    if (inBuffer_ == "error")
    {
        outBuffer_ = "";
        return false;
    }
    outBuffer_ = "已经收到数据:" + inBuffer_;
    return true;
}

void HandleData::handleWrite()
{
    if (outBuffer_.empty())
    {
        outBuffer_ = "error";
    }
    std::cout << "HandleData::handleWrite()--触发可写" << std::endl;
    if (writen(fd_, outBuffer_) < 0)
    {
        perror("writen");
        error_ = true;
        std::cout << "写入失败" << std::endl;
    }

    if (outBuffer_.size() > 0)
    {
        std::cout << "还没写完";
    }

    //loop_->updatePoller(channel_, 0);
}