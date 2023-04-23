#pragma once
#include <sys/epoll.h>

class Epoll;

class Channel{
private:
    Epoll *ep;
    int fd;
    uint32_t events;
    uint32_t revents;
    bool inEpoll;
public:
    Channel(Epoll*,int);
    ~Channel();

    void enable_reading();
    
    int getFd() const;
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void setInEpoll();
    void setRevents(uint32_t);
};
