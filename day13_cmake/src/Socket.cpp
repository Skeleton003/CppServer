#include "Socket.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>


Socket::Socket() : fd(-1){
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "socket create error");
}

Socket::Socket(int _fd) : fd(_fd){
    errif(fd == -1, "socket create error");
}

Socket::~Socket(){
    if(fd != -1){
        close(fd);
        fd = -1;
    }
}

void Socket::bind(InetAddress *_addr){
    sockaddr_in addr = _addr->getAddr();
    errif(::bind(fd, (sockaddr*)&addr, sizeof(addr)) == -1, "socket bind error");
}

void Socket::listen(){
    errif(::listen(fd, SOMAXCONN) == -1, "socket listen error");
}

void Socket::setnonblocking(){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

// for server socket
int Socket::accept(InetAddress *_addr){
    int clnt_sockfd = -1;
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    bzero(&addr,addr_len);
    if(fcntl(fd,F_GETFL) & O_NONBLOCK){ // non-block
        while(1){
            clnt_sockfd = ::accept(this->fd,(sockaddr*)&addr,&addr_len);
            if(clnt_sockfd!=-1){// success
                break;
            }
            if((errno == EAGAIN) || (errno == EWOULDBLOCK)){
                // printf("no connection yet\n");
                continue;
            }
            else{
                errif(true, "socket accept error");
            }
        }
    }
    else{
        clnt_sockfd = ::accept(fd, (sockaddr*)&addr, &addr_len);
        errif(clnt_sockfd == -1, "socket accept error");
    }
    _addr->setInetAddr(addr);
    return clnt_sockfd;
}

// for client socket
void Socket::connect(InetAddress *_addr){
    sockaddr_in addr = _addr->getAddr();
    if(fcntl(fd, F_GETFL) & O_NONBLOCK){
        while(1){
            int ret = ::connect(fd, (sockaddr*)&addr, sizeof(addr));
            if(ret == 0){
                break;
            } 
            else if(ret == -1 && (errno == EINPROGRESS)){
                continue;
            } 
            else if(ret == -1){
                errif(true, "socket connect error");
            }
        }
    }
    else{
        errif(::connect(fd, (sockaddr*)&addr, sizeof(addr)) == -1, "socket connect error");
    }
}

int Socket::getFd(){
    return fd;
}


InetAddress::InetAddress() {
    bzero(&addr, sizeof(addr));
}

InetAddress::InetAddress(const char* _ip, uint16_t _port) {
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(_ip);
    addr.sin_port = htons(_port);
}

InetAddress::~InetAddress(){
}


void InetAddress::setInetAddr(sockaddr_in _addr){
    addr = _addr;
}

sockaddr_in InetAddress::getAddr(){
    return addr;
}

char* InetAddress::getIp(){
    return inet_ntoa(addr.sin_addr);
}

uint16_t InetAddress::getPort(){
    return ntohs(addr.sin_port);
}