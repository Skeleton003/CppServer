#include "EventLoop.h"
#include "Server.h"
// #include <stdio.h>
#include <iostream>

int main(){
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    std::cout<<"server created\n";
    loop->loop();
    delete server;
    delete loop;

    return 0;
}