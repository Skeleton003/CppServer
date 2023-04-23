#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include <vector>

EventLoop::EventLoop() : ep(nullptr), quit(false){
    ep = new Epoll();
}

EventLoop::~EventLoop(){
    delete ep;
}

void EventLoop::loop(){
    while(!quit){
        std::vector<Channel*> chs = ep->poll();
        for(auto& ch:chs){
            ch->handle_event();
        }
    }
}

void EventLoop::updateChannel(Channel *_ch){
    ep->updateChannel(_ch);
}