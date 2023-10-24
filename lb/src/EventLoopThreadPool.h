#pragma once
#include <memory>
#include <vector>
#include "EventLoopThread.h"
#include "noncopyable.h"
#include <iostream>

class EventLoopThreadPool : noncopyable
{
public:
    EventLoopThreadPool(EventLoop *baseLoop, int numThreads);
    ~EventLoopThreadPool() { std::cout << "~EventLoopThreadPool()" << std::endl; }
    void start();
    EventLoop *getNextLoop();

private:
    EventLoop *baseLoop_;
    bool started_;
    int numThreads_;
    int next_;

    // 线程指针和对应的eventloop指针
    std::vector<std::shared_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop *> loops_;
};