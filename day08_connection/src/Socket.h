#pragma once

class InetAddress;

class Socket
{
private:
    int fd;
public:
    Socket();
    Socket(int _fd);
    ~Socket();

    void bind(InetAddress* _addr);
    void listen();
    void set_non_blocking();

    int accept(InetAddress*);

    int getFd();
};