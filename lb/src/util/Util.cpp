#include "Util.h"

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


const int MAX_BUFF = 4096;
ssize_t readn(int fd, void *buff, size_t n) {
  size_t nleft = n;
  ssize_t nread = 0;
  ssize_t readSum = 0;
  char *ptr = (char *)buff;
  while (nleft > 0) {
    if ((nread = read(fd, ptr, nleft)) < 0) {
      if (errno == EINTR)
        nread = 0;
      else if (errno == EAGAIN) {
        return readSum;
      } else {
        return -1;
      }
    } else if (nread == 0)
      break;
    readSum += nread;
    nleft -= nread;
    ptr += nread;
  }
  return readSum;
}

// 接受三个参数，分别是文件描述符 fd，一个字符串引用 inBuffer 以存储读取的数据，和一个布尔引用 zero 用于标识是否读取到了数据末尾。
ssize_t readn(int fd, std::string &inBuffer, bool &zero) {
  ssize_t nread = 0;
  ssize_t readSum = 0;
  while (true) {
    char buff[MAX_BUFF];
    if ((nread = read(fd, buff, MAX_BUFF)) < 0) {
      if (errno == EINTR)
        continue;
      else if (errno == EAGAIN) {
        return readSum;
      } else {
        perror("read error");
        return -1;
      }
    } else if (nread == 0) {
      // printf("redsum = %d\n", readSum);
      zero = true;
      break;
    }
    // printf("before inBuffer.size() = %d\n", inBuffer.size());
    // printf("nread = %d\n", nread);
    readSum += nread;
    // buff += nread;
    inBuffer += std::string(buff, buff + nread);
    // printf("after inBuffer.size() = %d\n", inBuffer.size());
  }
  return readSum;
}

ssize_t readn(int fd, std::string &inBuffer) {
  ssize_t nread = 0;
  ssize_t readSum = 0;
  while (true) {
    char buff[MAX_BUFF];
    if ((nread = read(fd, buff, MAX_BUFF)) < 0) {
      if (errno == EINTR)
        continue;
      else if (errno == EAGAIN) {
        return readSum;
      } else {
        perror("read error");
        return -1;
      }
    } else if (nread == 0) {
      // printf("redsum = %d\n", readSum);
      break;
    }
    // printf("before inBuffer.size() = %d\n", inBuffer.size());
    // printf("nread = %d\n", nread);
    readSum += nread;
    // buff += nread;
    inBuffer += std::string(buff, buff + nread);
    // printf("after inBuffer.size() = %d\n", inBuffer.size());
  }
  return readSum;
}

ssize_t writen(int fd, void *buff, size_t n) {
  size_t nleft = n;
  ssize_t nwritten = 0;
  ssize_t writeSum = 0;
  char *ptr = (char *)buff;
  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
      if (nwritten < 0) {
        if (errno == EINTR) {
          nwritten = 0;
          continue;
        } else if (errno == EAGAIN) {
          return writeSum;
        } else
          return -1;
      }
    }
    writeSum += nwritten;
    nleft -= nwritten;
    ptr += nwritten;
  }
  return writeSum;
}

// 用于向文件描述符中写入指定长度的数据
ssize_t writen(int fd, std::string &sbuff) {
  size_t nleft = sbuff.size();  // 剩余要写入的字节数
  ssize_t nwritten = 0;  // 已写入的字节数
  ssize_t writeSum = 0;  // 总共写入的字节数
  const char *ptr = sbuff.c_str(); // 待写入数据的指针
  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
      if (nwritten < 0) {
        if (errno == EINTR) { // 如果写入被信号中断，则重新尝试写入
                              // EINTR 错误（Interrupted system call，系统调用被中断）
                              // 意味着在进行写入操作时发生了一个被信号中断的事件
          nwritten = 0;
          continue;
        } else if (errno == EAGAIN){// 如果出现非阻塞操作错误 
                                    // EAGAIN 错误（Resource temporarily unavailable，资源暂时不可用）
                                    // 通常表示写入的文件描述符是非阻塞的，并且当前无法立即完成写操作
                                    // 当 write 函数返回 EAGAIN 错误时，会跳出循环，停止继续写入数据
                                    // 这里的处理逻辑是假设非阻塞操作错误表示当前无法立即完成写入，并且暂时不进行重试
                                    // 值得注意的是，在这段代码中并没有对 EAGAIN 错误进行特定的处理或者等待一段时间后重试写入
                                    // 这意味着当遇到 EAGAIN 错误时，函数会直接退出循环，并返回已写入的字节数
          break;
        }
        else
          return -1;
      }
    }
    writeSum += nwritten;  // 更新已写入的字节数
    nleft -= nwritten;  // 更新剩余要写入的字节数
    ptr += nwritten;  // 更新待写入数据的指针
  }
  if (writeSum == static_cast<int>(sbuff.size()))
    sbuff.clear(); // 如果所有数据都成功写入，则清空字符串
  else
    sbuff = sbuff.substr(writeSum); // 如果部分数据写入成功，则更新字符串，保留未写入的部分
  return writeSum; // 返回总共写入的字节数
}

// SIGPIPE 这个信号是很常见的
// 当往一个写端关闭的管道或socket连接中连续写入数据时会引发SIGPIPE信号

// 处理SIGPIPE信号，忽略
void handle_for_sigpipe() {
  struct sigaction sa;
  memset(&sa, '\0', sizeof(sa));
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  if (sigaction(SIGPIPE, &sa, NULL)) return;
}

// 设置文件描述符非阻塞
int setSocketNonBlocking(int fd) {
  int flag = fcntl(fd, F_GETFL, 0);
  if (flag == -1) return -1;

  flag |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flag) == -1) return -1;
  return 0;
}

// 注下：
// https://zhuanlan.zhihu.com/p/48794942

// 禁用nagle算法
void setSocketNodelay(int fd) {
  int enable = 1;
  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable));
}

// 注下：
// 优雅的关闭连接
// https://blog.csdn.net/qq_36316285/article/details/115287746


// 通过setsockopt可以设置SO_LINGER,从而实现优雅的关闭连接
void setSocketNoLinger(int fd) {
  struct linger linger_;
  linger_.l_onoff = 1;
  linger_.l_linger = 30;
  setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char *)&linger_,
             sizeof(linger_));
}

// 断开输出流，发送FIN报文结束连接
void shutDownWR(int fd) {
  shutdown(fd, SHUT_WR);
  // printf("shutdown\n");
}

// socket绑定到port，返回监听的文件描述符listen_fd
int socket_bind_listen(int port) {
  // 检查port值，取正确区间范围
  if (port < 0 || port > 65535) return -1;

  // 创建socket(IPv4 + TCP)，返回监听描述符
  int listen_fd = 0;
  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) return -1;

  // 消除bind时"Address already in use"错误
  int optval = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval,
                 sizeof(optval)) == -1) {
    close(listen_fd);
    return -1;
  }

  // 设置服务器IP和Port，和监听描述副绑定
  struct sockaddr_in server_addr;
  bzero((char *)&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons((unsigned short)port);
  if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    close(listen_fd);
    return -1;
  }

  // 开始监听，最大等待队列长为LISTENQ
  if (listen(listen_fd, 2048) == -1) {
    close(listen_fd);
    return -1;
  }

  // 无效监听描述符
  if (listen_fd == -1) {
    close(listen_fd);
    return -1;
  }
  return listen_fd;
}