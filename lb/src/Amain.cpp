#include <getopt.h>
#include <string>
#include <iostream>
#include "EventLoop.h"
#include "Server.h"

int main(int argc, char *argv[])
{
    int threadNum = 4;
    int port = 9999;
    std::string logPath = "./WebServer.log";
// Logger::setLogFileName(logPath);

// STL库在多线程上应用
#ifndef _PTHREADS
    std::cout << "_PTHREADS is not defined !" << std::endl;
    ;
#endif
    std::cout << "port = " << 9999 << std::endl;

    EventLoop mainLoop;
    Server myServer(&mainLoop, threadNum, port);
    myServer.start();
    mainLoop.loop();
    return 0;
}
