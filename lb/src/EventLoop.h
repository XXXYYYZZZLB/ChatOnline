#pragma once
#include <memory>
#include "Epoll.h"
#include "Channel.h"
#include <mutex>

class Channel;
class Epoll;
typedef std::shared_ptr<Channel> SP_Channel;

class EventLoop
{
private:
    typedef std::function<void()> Function;
    bool looping_;
    bool quit_;

    int wakeupFd_;
    std::shared_ptr<Channel> pwakeupChannel_;

    const pid_t threadId_;
    std::shared_ptr<Epoll> poller_;

    mutable std::mutex mutex_;
    std::vector<Function> pendingFunctors_; // 待执行队列

    void wakeup();
    void handleWakeup();

public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void runInLoop(Function &&func);
    void queueInLoop(Function &&func);

    bool isInLoopThread() const;
    void assertInLoopThread();
    void shutdown(SP_Channel channel);

    void removeFromPoller(SP_Channel channel);
    void updatePoller(SP_Channel channel, int timeout = 0);
    void addToPoller(SP_Channel channel, int timeout = 0);
};
