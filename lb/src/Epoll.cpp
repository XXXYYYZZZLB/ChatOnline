#include "Epoll.h"
#include <assert.h>
#include <iostream>

const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;

Epoll::Epoll() : epollFd_(epoll_create1(EPOLL_CLOEXEC)), vecEvents_(EVENTSNUM)
{
  assert(epollFd_ > 0);
}
Epoll::~Epoll() {}

void Epoll::epoll_add(SP_Channel request, int timeout)
{
  int fd = request->getFd();
  struct epoll_event event;
  event.data.fd = fd;
  event.events = request->getEvents();

  fd2chan_[fd] = request;
  if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0)
  {
    perror("epoll_add error");
    fd2chan_[fd].reset();
  }
}

void Epoll::epoll_mod(SP_Channel request, int timeout)
{
  int fd = request->getFd();
  struct epoll_event event;
  event.data.fd = fd;
  event.events = request->getEvents();

  if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0)
  {
    perror("epoll_mod error");
    fd2chan_[fd].reset();
  }
}

void Epoll::epoll_del(SP_Channel request)
{
  int fd = request->getFd();
  struct epoll_event event;
  event.data.fd = fd;
  event.events = request->getEvents();

  if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0)
  {
    perror("epoll_del error");
  }
  fd2chan_[fd].reset();
}

// 返回活跃事件数
std::vector<SP_Channel> Epoll::poll()
{
  while (true)
  {
    int event_count =
        epoll_wait(epollFd_, &*vecEvents_.begin(), vecEvents_.size(), EPOLLWAIT_TIME);
    if (event_count < 0)
      perror("epoll wait error");
    std::vector<SP_Channel> req_data = getEventsRequest(event_count);
    if (req_data.size() > 0)
      return req_data;
  }
}

std::vector<SP_Channel> Epoll::getEventsRequest(int events_num)
{
  std::vector<SP_Channel> req_data;
  for (int i = 0; i < events_num; ++i)
  {
    // 获取有事件产生的描述符
    int fd = vecEvents_[i].data.fd;

    SP_Channel cur_req = fd2chan_[fd];

    if (cur_req)
    {
      cur_req->setRevents(vecEvents_[i].events);
      cur_req->setEvents(0);
      req_data.push_back(cur_req);
    }
    else
    {
      std::cout << "SP cur_req is invalid";
    }
  }
  return req_data;
}