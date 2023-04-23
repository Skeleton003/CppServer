#include "src/EventLoop.h"
#include "src/Server.h"
#include <stdio.h>

int main(){
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    printf("server created\n");
    loop->loop();


    return 0;
}