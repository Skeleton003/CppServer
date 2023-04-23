#pragma once

class InetAddress;

class Socket
{
private:
    int fd;
public:
    Socket();
    Socket(int);
    ~Socket();

    void bind(InetAddress*);
    void listen();
    void set_non_blocking();

    int accept(InetAddress*);

    int getFd();
};