#pragma once
#include <functional>
#include <memory>
#include "EventLoop.h"
#include "HandleData.h"

class HandleData;
class EventLoop;

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

    EventLoop *loop_;

    // 方便找到上层持有该Channel的对象
    std::weak_ptr<HandleData> holder_;

public:
    Channel();
    Channel(int fd);
    Channel(EventLoop *loop);
    Channel(EventLoop *loop, int fd);
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

    // setHolder 和 getHolder 方法用于设置和获取持有该 Channel 对象的 HttpData 对象
    void setHolder(std::shared_ptr<HandleData> holder);
    std::shared_ptr<HandleData> getHolder();
};

typedef std::shared_ptr<Channel> SP_Channel;
