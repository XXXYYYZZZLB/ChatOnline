// CurrentThread.h
#pragma once

namespace CurrentThread {
  extern __thread int t_cachedTid;
  extern __thread char t_tidString[32];
  extern __thread int t_tidStringLength;
  extern __thread const char* t_threadName;

  void cacheTid();
  int tid();
  const char* tidString();
  int tidStringLength();
  const char* name();
}
