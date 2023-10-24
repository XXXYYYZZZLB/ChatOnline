#pragma once
#include <sys/epoll.h>
#include <vector>
#include <memory>
#include "Channel.h"

class Channel;
typedef std::shared_ptr<Channel> SP_Channel;

class Epoll
{
private:
    static const int MAX_FD = 100000;
    int epollFd_;
    std::vector<epoll_event> vecEvents_;
    SP_Channel fd2chan_[MAX_FD];

public:
    Epoll();
    ~Epoll();

    void epoll_add(SP_Channel request, int timeout=0);
    void epoll_mod(SP_Channel request, int timeout=0);
    void epoll_del(SP_Channel request);
    
    int getEpollFd();
    
    std::vector<SP_Channel> poll();
    std::vector<SP_Channel> getEventsRequest(int events_num);
};
