#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<errno.h>
#include"util.h"

#define MAX_EVENTS 1024
#define MAX_BUFFER 1024

void set_non_blocking(int fd){
    fcntl(fd,F_SETFL,fcntl(fd,F_GETFL)|O_NONBLOCK);
}

void handle_event(int fd){
    char buf[MAX_BUFFER];
    while(1){
        bzero(buf,sizeof(buf));
        ssize_t bytes_read = read(fd,buf,sizeof(buf));
        if(bytes_read>0){
            printf("message from client fd %d: %s\n",fd,buf);
            write(fd,buf,sizeof(buf));
        }
        else if(bytes_read==-1 && errno==EINTR){
            printf("continue reading\n");
            continue;
        }
        else if(bytes_read==-1 && ((errno==EAGAIN)||(errno==EWOULDBLOCK))){
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

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd==-1,"socket create error");
    printf("socket create successful\n");

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8888);

    errif(bind(sockfd, (sockaddr*)&server_addr, sizeof(server_addr))==-1,"socket bind error");

    errif(listen(sockfd, SOMAXCONN)==-1,"socket listen error");

    int epfd = epoll_create1(0);
    errif(epfd==-1, "epoll create error");
    printf("epoll create successful\n");

    struct epoll_event events[MAX_EVENTS],ev;
    bzero(events,sizeof(events));
    bzero(&ev,sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    set_non_blocking(sockfd);
    epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&ev);

    while(1){
        int nfds = epoll_wait(epfd,events,MAX_EVENTS,-1);
        errif(nfds==-1,"epoll wait error");
        // printf("nfds = %d\n",nfds);
        for(int i=0;i<nfds;++i){
            if(events[i].data.fd==sockfd){//新客户端建立连接
                sockaddr_in client_addr;
                socklen_t client_addr_len=sizeof(client_addr);
                bzero(&client_addr,client_addr_len);

                int client_sockfd = accept(sockfd,(sockaddr*)&client_addr,&client_addr_len);
                errif(client_sockfd==-1,"socket accept error");
                printf("new client fd %d! IP: %s Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                bzero(&ev,sizeof(ev));
                ev.data.fd = client_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                set_non_blocking(client_sockfd);
                epoll_ctl(epfd,EPOLL_CTL_ADD,client_sockfd,&ev);
            }
            else if(events[i].events & EPOLLIN){//可读事件
                handle_event(events[i].data.fd);
            }
            else{
                printf("something else happened\n");
            }
        }
    }
    close(sockfd);

    return 0;
}