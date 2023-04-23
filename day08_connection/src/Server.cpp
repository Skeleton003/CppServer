#include "Server.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#define READ_BUFFER 1024

Server::Server(EventLoop *_loop):loop(_loop){
    acceptor = new Acceptor(loop);
    acceptor->setNewConnectionCallback(std::bind(&Server::newConnection,this,std::placeholders::_1));
}

Server::~Server(){
    delete acceptor;
}

// void Server::handle_read_event(int sockfd){
//     char buf[READ_BUFFER];
//     while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
//         bzero(&buf, sizeof(buf));
//         ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
//         if(bytes_read > 0){
//             printf("message from client fd %d: %s\n", sockfd, buf);
//             write(sockfd, buf, sizeof(buf));
//         } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
//             printf("continue reading");
//             continue;
//         } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
//             printf("finish reading once, errno: %d\n", errno);
//             break;
//         } else if(bytes_read == 0){  //EOF，客户端断开连接
//             printf("EOF, client fd %d disconnected\n", sockfd);
//             close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
//             break;
//         }
//     }
// }

void Server::newConnection(Socket *sock){
    Connection *conn = new Connection(loop,sock);
    conn->setDeleteConnectionCallback(std::bind(&Server::deleteConnection,this,std::placeholders::_1));
    connections[sock->getFd()]=conn;
}

void Server::deleteConnection(Socket *sock){
    delete connections[sock->getFd()];
    connections.erase(sock->getFd());
}