#pragma once
#include <functional>
#include <memory>

// Fd 包装类
class Channel
{
private:
    typedef std::function<void()> CallBack;

    int fd_;
    uint32_t events_;  // 设置监听的事件
    uint32_t revents_; // 监听到的事件

    CallBack readHandler_;
    CallBack writeHandler_;
    CallBack errorHandler_;

public:
    Channel();
    Channel(int fd);
    ~Channel();

    int getFd();
    void setFd(int fd);

    void setRevents(__uint32_t ev);
    void setEvents(__uint32_t ev);
    __uint32_t getEvents();
    __uint32_t getRevents();

    void setReadHandler(CallBack &&func);
    void setWriteHandler(CallBack &&func);
    void setErrorHandler(CallBack &&func);
    void handleEvents(); // 根据监听到是事件执行回调函数

    void handleRead();
    void handleWrite();
    void handleError();
};

typedef std::shared_ptr<Channel> SP_Channel;
