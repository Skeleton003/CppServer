#pragma once
#include <sys/epoll.h>
#include <vector>

class Epoll{
private:
    int epfd;
    epoll_event *events;
public:
    Epoll();
    ~Epoll();

    void addFd(int,uint32_t);
    std::vector<epoll_event> poll(int timeout=-1);
};