#include <iostream>
#include <unistd.h>
#include <string.h>
#include "src/util.h"
#include "src/Buffer.h"
#include "src/Socket.h"

#define MAX_BUFFER 1024
using namespace std;

int main(){
    Socket *sock = new Socket();
    InetAddress *addr = new InetAddress("127.0.0.1", 1234);
    sock->connect(addr);
    printf("server connected\n");

    int sockfd = sock->getFd();
    Buffer *sendBuffer = new Buffer();
    Buffer *readBuffer = new Buffer();

    while(1){
        sendBuffer->getline();
        // printf("now have read: %d\n",(int)sendBuffer->size());
        if(sendBuffer->size()==0){
            fprintf(stderr,"please input something\n");
            continue;
        }
        ssize_t write_bytes = write(sockfd,sendBuffer->c_str(),sendBuffer->size());
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        // printf("now have wrote: %d\n",(int)write_bytes);
        int already_read = 0;
        char buf[MAX_BUFFER];
        while(1){
            // printf("already_read: %d\n",already_read);
            bzero(&buf, sizeof(buf));
            ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
            if(read_bytes > 0){
                readBuffer->append(buf, read_bytes);
                already_read += read_bytes;
            }
            else if(read_bytes == 0){         //EOF
                printf("server disconnected!\n");
                exit(EXIT_SUCCESS);
            }
            if(already_read >= sendBuffer->size()){
                printf("message from server: %s\n", readBuffer->c_str());
                break;
            } 
        }
        readBuffer->clear();
    }
    delete readBuffer;
    delete sendBuffer;
    delete addr;
    delete sock;

    return 0;
}