#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include <vector>
#include <stdio.h>

EventLoop::EventLoop() : ep(nullptr), quit(false){
    ep = new Epoll();
}

EventLoop::~EventLoop(){
    delete ep;
}

void EventLoop::loop(){
    while(!quit){
        std::vector<Channel*> chs = ep->poll();
        // printf("nfds: %d\n",(int)chs.size());
        for(auto& ch:chs){  
            // printf("ch_fd: %d\n",ch->getFd());
            ch->handle_event();
        }
    }
}

void EventLoop::updateChannel(Channel *_ch){
    ep->updateChannel(_ch);
}