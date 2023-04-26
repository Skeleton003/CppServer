#pragma once
#include <map>
#include <vector>
class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;

class Server{
private:
    EventLoop *mainReactor;     //只负责接受连接，然后分发给一个subReactor
    Acceptor *acceptor;                     //连接接受器
    std::map<int, Connection*> connections; //TCP连接
    std::vector<EventLoop*> subReactors;    //负责处理事件循环
    ThreadPool *thpool;     //线程池
public:
    Server(EventLoop*);
    ~Server();

    void handleReadEvent(int);
    void newConnection(Socket *sock);
    void deleteConnection(int sockfd);
};