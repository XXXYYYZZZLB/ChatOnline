#include "EventLoopThread.h"
#include <functional>

EventLoopThread::EventLoopThread()
    : loop_(NULL),
      exiting_(false),
      thread_(std::mem_fun(&EventLoopThread::threadFunc), this),
      mutex_(),
      cond_() {}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if (loop_ != NULL)
    {
        loop_->quit();
        thread_.join();
    }
}

EventLoop *EventLoopThread::startLoop()
{
    // assert(!thread_.started());
    // thread_.start();
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (loop_ == NULL)
            cond_.wait(lock);
    }
    return loop_;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_all();
    }
    loop.loop();
    loop_ = NULL;
}