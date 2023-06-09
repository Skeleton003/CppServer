#include "Server.h"
#include "Socket.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include "EventLoop.h"
#include <unistd.h>
#include <functional>

Server::Server(EventLoop *_loop):mainReactor(_loop),acceptor(nullptr){
    acceptor = new Acceptor(mainReactor); // Acceptor由且只由mainReactor负责
    acceptor->setNewConnectionCallback(std::bind(&Server::newConnection,this,std::placeholders::_1));

    int size = std::thread::hardware_concurrency();     //线程数量，也是subReactor数量
    thpool = new ThreadPool(size);
    for(int i=0;i<size;++i){
        subReactors.push_back(new EventLoop());// 每一个线程是一个EventLoop
    }

    for(int i=0;i<size;++i){
        std::function<void()> sub_loop = std::bind(&EventLoop::loop,subReactors[i]);
        thpool->add(sub_loop);// 开启所有线程的事件循环
    }
}

Server::~Server(){
    delete acceptor;
    delete thpool;
    for(auto lp:subReactors)delete lp;
}

void Server::newConnection(Socket *sock){
    if(sock->getFd() != -1){
        int random = sock->getFd()%subReactors.size();
        Connection *conn = new Connection(subReactors[random],sock);
        conn->setDeleteConnectionCallback(
            std::bind(&Server::deleteConnection,this,std::placeholders::_1)
        );
        connections[sock->getFd()] = conn;
    }
}

void Server::deleteConnection(int sockfd){
    if(sockfd!=-1){
        auto it = connections.find(sockfd);
        if(it!=connections.end()){
            Connection *conn = connections[sockfd];
            connections.erase(sockfd);
            close(sockfd);
        }
    }
}