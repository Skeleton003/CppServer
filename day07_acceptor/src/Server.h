#pragma once

class EventLoop;
class Socket;
class Acceptor;

class Server{
private:
    EventLoop *loop;
    Acceptor *acceptor;
public:
    Server(EventLoop *_loop);
    ~Server();

    void handle_read_event(int);
    void newConnection(Socket *serv_sock);
};