#pragma once
#include <sys/epoll.h>
#include <vector>

class Channel;

class Epoll{
private:
    int epfd;
    epoll_event *events;
public:
    Epoll();
    ~Epoll();

    // void addFd(int,uint32_t);
    void updateChannel(Channel*);
    // std::vector<epoll_event> poll(int timeout=-1);
    std::vector<Channel*> poll(int timeout=-1);
};