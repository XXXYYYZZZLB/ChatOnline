#pragma once
#include "EventLoop.h"
#include "Channel.h"
#include "HandleData.h"
#include <string>

class Channel;
class EventLoop;
typedef std::shared_ptr<Channel> SP_Channel;

class HandleData
{
private:
  EventLoop *loop_;
  SP_Channel channel_;
  int fd_;
  std::string inBuffer_;
  std::string outBuffer_;
  bool error_;

  void handleRead();
  void handleWrite();
  bool analysisRequest();

  std::string data;

public:
  HandleData(EventLoop *loop, int connfd);
  ~HandleData();

  SP_Channel getChannel();
  EventLoop *getLoop();

  void newEvent();
};