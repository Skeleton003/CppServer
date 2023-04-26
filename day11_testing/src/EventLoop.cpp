#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "ThreadPool.h"
#include <vector>
#include <stdio.h>

EventLoop::EventLoop() : ep(nullptr), threadPool(nullptr), quit(false){
    ep = new Epoll();
    threadPool = new ThreadPool();
}

EventLoop::~EventLoop(){
    delete ep;
    delete threadPool;
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

void EventLoop::addThread(std::function<void()> func){
    threadPool->add(func);
}