#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include <stdio.h>
// #include "Server.h"

Acceptor::Acceptor(EventLoop *_loop) : loop(_loop)
{
    sock = new Socket();
    InetAddress *addr = new InetAddress("127.0.0.1", 8888);
    sock->bind(addr);
    sock->listen(); 
    sock->set_non_blocking();
    acceptChannel = new Channel(loop, sock->getFd());
    acceptChannel->setCallback(std::bind(&Acceptor::acceptConnection, this));
    acceptChannel->enable_reading();
    delete addr;
}

Acceptor::~Acceptor(){
    delete sock;
    // delete addr;
    delete acceptChannel;
}

void Acceptor::acceptConnection(){
    // newConnectionCallback(sock);
    InetAddress *clnt_addr = new InetAddress();
    Socket *clnt_sock = new Socket(this->sock->accept(clnt_addr));
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->getAddr().sin_addr), ntohs(clnt_addr->getAddr().sin_port));
    clnt_sock->set_non_blocking();
    newConnectionCallback(clnt_sock);
    delete clnt_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> _cb){
    newConnectionCallback = _cb;
}