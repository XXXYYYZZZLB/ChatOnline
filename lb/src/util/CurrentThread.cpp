#include "CurrentThread.h"
#include <sys/syscall.h>
#include <cstdio>
#include <sys/types.h>
#include <unistd.h> 

namespace CurrentThread {
  __thread int t_cachedTid = 0;
  __thread char t_tidString[32];
  __thread int t_tidStringLength = 6;
  __thread const char* t_threadName = "default";

  void cacheTid() {
    if (t_cachedTid == 0) {
      t_cachedTid = static_cast<int>(syscall(SYS_gettid));
      t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
    }
  }

  int tid() {
    if (t_cachedTid == 0) {
      cacheTid();
    }
    return t_cachedTid;
  }

  const char* tidString() {
    return t_tidString;
  }

  int tidStringLength() {
    return t_tidStringLength;
  }

  const char* name() {
    return t_threadName;
  }
}
