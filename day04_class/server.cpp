#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include "util.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void handle_read_event(int);

signed main(){
    Socket *serv_sock=new Socket();
    InetAddress *serv_addr=new InetAddress("127.0.0.1",8888);
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    serv_sock->set_non_blocking();
    printf("socket created\n");

    Epoll *ep=new Epoll();
    ep->addFd(serv_sock->getFd(),EPOLLIN | EPOLLET);
    printf("epoll created\n");

    while(1){
        std::vector<epoll_event> events = ep->poll();
        int nfds = events.size();
        for(int i=0;i<nfds;++i){
            auto& ev=events[i];
            if(ev.data.fd==serv_sock->getFd()){
                InetAddress *clnt_addr=new InetAddress();
                Socket *clnt_sock=new Socket(serv_sock->accept(clnt_addr));
                printf("new client fd %d! IP: %s Port: %d\n", \
                clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
                clnt_sock->set_non_blocking();
                ep->addFd(clnt_sock->getFd(),EPOLLIN | EPOLLET);
            }
            else if(ev.events & EPOLLIN){
                handle_read_event(ev.data.fd);
            }
            else printf("something else happened\n");
        }
    }
    delete serv_addr;
    delete serv_sock;

    return 0;
}

void handle_read_event(int fd){
    char buf[READ_BUFFER];
    while(1){
        bzero(buf,sizeof(buf));
        ssize_t bytes_read = read(fd,buf,sizeof(buf));
        if(bytes_read>0){
            printf("message from client fd %d: %s\n", fd, buf);
            write(fd, buf, sizeof(buf));
        }
        else if(bytes_read==-1 && errno==EINTR){
            printf("continue reading");
            continue;
        }
        else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
            printf("finish reading once, errno: %d\n", errno);
            break;
        }
        else if(bytes_read==0){
            printf("EOF, client fd %d disconnected\n", fd);
            close(fd);
            break;
        }
    }
}