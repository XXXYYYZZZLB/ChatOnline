#include "EventLoop.h"
#include <sys/eventfd.h>
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>
#include "util/Util.h"
#include "util/CurrentThread.h"

namespace
{
    int createEventfd()
    {
        int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (evtfd < 0)
        {
            abort();
        }
        return evtfd;
    }
}

__thread EventLoop *t_loopInThisThread = nullptr;

EventLoop::EventLoop()
    : looping_(false),
      poller_(new Epoll()),
      wakeupFd_(createEventfd()),
      quit_(false),
      threadId_(CurrentThread::tid()),
      pwakeupChannel_(new Channel(wakeupFd_))

{
    if (t_loopInThisThread == nullptr)
    {
        t_loopInThisThread = this;
    }
    pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
    pwakeupChannel_->setReadHandler(std::bind(&EventLoop::handleWakeup, this));
    poller_->epoll_add(pwakeupChannel_, 0);
}
EventLoop::~EventLoop()
{
    close(wakeupFd_);
    t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
    assert(!looping_);
    assert(isInLoopThread());

    looping_ = true;
    quit_ = false;

    std::vector<SP_Channel> ret;
    while (!quit_)
    {
        ret.clear();
        ret = poller_->poll();

        // 处理channel对应的注册事件
        for (auto &it : ret)
            it->handleEvents();

        // 处理任务队列
        std::vector<Function> functors;
        {
            std::lock_guard<std::mutex> guard(mutex_);
            functors.swap(pendingFunctors_);
        }
        for (size_t i = 0; i < functors.size(); ++i)
            functors[i]();
    }
    looping_ = false;
}
void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::runInLoop(Function &&func)
{
    if (isInLoopThread())
        func();
    else
        queueInLoop(std::move(func));
}

void EventLoop::queueInLoop(Function &&func)
{
    {
        std::lock_guard<std::mutex> guard(mutex_);
        pendingFunctors_.emplace_back(std::move(func));
    }
    if (!isInLoopThread())
        wakeup();
}

bool EventLoop::isInLoopThread() const
{
    return threadId_ == CurrentThread::tid();
}

void EventLoop::assertInLoopThread()
{
    assert(isInLoopThread());
}

void EventLoop::shutdown(SP_Channel channel)
{
    int fd = channel->getFd();
    ::shutdown(fd, SHUT_WR);
}

void EventLoop::removeFromPoller(SP_Channel channel)
{
    poller_->epoll_del(channel);
}
void EventLoop::updatePoller(SP_Channel channel, int timeout)
{
    poller_->epoll_mod(channel, timeout);
}
void EventLoop::addToPoller(SP_Channel channel, int timeout)
{
    poller_->epoll_add(channel, timeout);
}

void EventLoop::wakeup() {
  uint64_t one = 1;
  ssize_t n = writen(wakeupFd_, (char*)(&one), sizeof one);
  if (n != sizeof one) {
    std::cout << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
  }
}

void EventLoop::handleWakeup()
{
    uint64_t one = 1;
    ssize_t n = readn(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        std::cout << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
    // pwakeupChannel_->setEvents(EPOLLIN | EPOLLET | EPOLLONESHOT);
    pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
}