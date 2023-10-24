#include "Server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <functional>
#include <unistd.h>
#include "util/Util.h"
#include <string.h>
#include <iostream>
#include "HandleData.h"

Server::Server(EventLoop *loop, int threadNum, int port)
    : loop_(loop),
      threadNum_(threadNum),
      eventLoopThreadPool_(new EventLoopThreadPool(loop_, threadNum)),
      started_(false),
      acceptChannel_(new Channel(loop_)),
      port_(port),
      listenFd_(socket_bind_listen(port_))
{
    acceptChannel_->setFd(listenFd_);
    handle_for_sigpipe();
    if (setSocketNonBlocking(listenFd_) < 0)
    {
        std::cout << listenFd_ << std::endl;
        perror("set socket non block failed");
        abort();
    }
}

void Server::start()
{
    eventLoopThreadPool_->start();
    // acceptChannel_->setEvents(EPOLLIN | EPOLLET | EPOLLONESHOT);
    acceptChannel_->setEvents(EPOLLIN | EPOLLET);
    acceptChannel_->setReadHandler(std::bind(&Server::handNewConn, this));
    // acceptChannel_->setConnHandler(bind(&Server::handThisConn, this));
    loop_->addToPoller(acceptChannel_, 0);
    started_ = true;
}

void Server::handNewConn()
{
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = sizeof(client_addr);
    int accept_fd = 0;
    while ((accept_fd = accept(listenFd_, (struct sockaddr *)&client_addr,
                               &client_addr_len)) > 0)
    {
        EventLoop *loop = eventLoopThreadPool_->getNextLoop();
        std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":"
                  << ntohs(client_addr.sin_port) << std::endl;
        std::cout << "new connection" << std::endl;
        std::cout << inet_ntoa(client_addr.sin_addr) << std::endl;
        std::cout << ntohs(client_addr.sin_port) << std::endl;
        /*
        // TCP的保活机制默认是关闭的
        int optval = 0;
        socklen_t len_optval = 4;
        getsockopt(accept_fd, SOL_SOCKET,  SO_KEEPALIVE, &optval, &len_optval);
        cout << "optval ==" << optval << endl;
        */
        // 限制服务器的最大并发连接数
        if (accept_fd >= MAXFDS)
        {
            close(accept_fd);
            continue;
        }
        // 设为非阻塞模式
        if (setSocketNonBlocking(accept_fd) < 0)
        {
            std::cout << "Set non block failed!" << std::endl;
            ;
            // perror("Set non block failed!");
            return;
        }

        setSocketNodelay(accept_fd);

        std::shared_ptr<HandleData> req_info(new HandleData(loop, accept_fd));
        req_info->getChannel()->setHolder(req_info);
        loop->queueInLoop(std::bind(&HandleData::newEvent, req_info));
    }
    acceptChannel_->setEvents(EPOLLIN | EPOLLET);
}