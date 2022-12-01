#include <sys/epoll.h>
#include <iostream>
#include <fcntl.h>
#include <sys/socket.h> /*socket*/
#include <netinet/in.h> /*sockaddr_in*/
#include <string.h> /*bzero*/
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>

void setnonblocking(int sock)
{
     int opts;
     opts=fcntl(sock,F_GETFL);
     if(opts<0)
     {
          perror("fcntl(sock,GETFL)");
          exit(1);
     }
     opts = opts|O_NONBLOCK;
     if(fcntl(sock,F_SETFL,opts)<0)
     {
          perror("fcntl(sock,SETFL,opts)");
          exit(1);
     }  
}

void addFd(int epollFd, int fd, bool oneshot){
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    if(oneshot){
        event.events |=EPOLLONESHOT;
    }
    epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

int epollfd = epoll_create(5);
epoll_event events[100];
int main(){
    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    /* 创建监听socket的TCP/IP的IPV4 socket地址 */
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY：将套接字绑定到所有可用的接口 */
    address.sin_port = htons(5005);

    int flag = 1;
    /* SO_REUSEADDR 允许端口被重复使用 */
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    int ret;
    /* 绑定socket和它的地址 */
    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));  
    /* 创建监听队列以存放待处理的客户连接，在这些客户连接被accept()之前 */
    ret = listen(listenfd, 5);
    addFd(epollfd, listenfd, false);
    int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
    /* 然后我们遍历这一数组以处理这些已经就绪的事件 */
    for(int i = 0; i < number; ++i) {
        int sockfd = events[i].data.fd;  // 事件表中就绪的socket文件描述符
        if(sockfd == listenfd) {  // 当listen到新的用户连接，listenfd上则产生就绪事件
            struct sockaddr_in client_address;
            socklen_t client_addrlength = sizeof(client_address);
            /* ET模式 */
            while(1) {
                /* accept()返回一个新的socket文件描述符用于send()和recv() */
                int connfd = accept(listenfd, (struct sockaddr *) &client_address, &client_addrlength);
                /* 并将connfd注册到内核事件表中 */
                // users[connfd].init(connfd, client_address);
                /* ... */
            }
        }
        else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
            // 如有异常，则直接关闭客户连接，并删除该用户的timer
            /* ... */
        }
        else if(events[i].events & EPOLLIN) {
            /* 当这一sockfd上有可读事件时，epoll_wait通知主线程。*/
            // if(users[sockfd].read()) { /* 主线程从这一sockfd循环读取数据, 直到没有更多数据可读 */
                // pool->append(users + sockfd);  /* 然后将读取到的数据封装成一个请求对象并插入请求队列 */
                /* ... */
            // }
            // else
                /* ... */
        }
        else if(events[i].events & EPOLLOUT) {
            /* 当这一sockfd上有可写事件时，epoll_wait通知主线程。主线程往socket上写入服务器处理客户请求的结果 */
            // if(users[sockfd].write()) {
                /* ... */
            // }
            // else
                /* ... */
        }
    }
    return 0;
}
