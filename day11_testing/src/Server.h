#pragma once

#include <map>
class EventLoop;
class Socket;
class Acceptor;
class Connection;

class Server{
private:
    EventLoop *loop;
    Acceptor *acceptor;
    std::map<int,Connection*> connections;
public:
    Server(EventLoop *_loop);
    ~Server();

    void handle_read_event(int);
    void newConnection(Socket *sock);
    void deleteConnection(int sockfd);
};