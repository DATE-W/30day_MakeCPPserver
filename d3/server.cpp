#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "util.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

// 设置套接字为非阻塞模式
void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main() {
    // 创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket 创建失败");

    // 配置服务器地址
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器的IP地址
    serv_addr.sin_port = htons(8888); // 服务器的端口号

    // 绑定套接字到服务器地址
    errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket 绑定失败");

    // 监听连接请求
    errif(listen(sockfd, SOMAXCONN) == -1, "socket 监听失败");
    
    // 创建 epoll 实例
    int epfd = epoll_create1(0);
    errif(epfd == -1, "epoll 创建失败");

    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events));

    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET; // 设置监听可读事件，EPOLLET 表示边缘触发模式
    setnonblocking(sockfd); // 设置套接字为非阻塞模式
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev); // 将套接字添加到 epoll 监听中

    while(true){
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1); // 等待事件发生
        errif(nfds == -1, "epoll 等待失败");
        for(int i = 0; i < nfds; ++i){
            if(events[i].data.fd == sockfd){        // 新客户端连接
                struct sockaddr_in clnt_addr;
                bzero(&clnt_addr, sizeof(clnt_addr));
                socklen_t clnt_addr_len = sizeof(clnt_addr);

                int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len); // 接受客户端连接
                errif(clnt_sockfd == -1, "socket 接受连接失败");
                printf("新客户端套接字文件描述符：%d，IP 地址：%s 端口号：%d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

                bzero(&ev, sizeof(ev));
                ev.data.fd = clnt_sockfd;
                ev.events = EPOLLIN | EPOLLET; // 设置监听可读事件，EPOLLET 表示边缘触发模式
                setnonblocking(clnt_sockfd); // 设置套接字为非阻塞模式
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev); // 将新客户端套接字添加到 epoll 监听中
            } 
            else if(events[i].events & EPOLLIN){      // 可读事件
                char buf[READ_BUFFER];
                while(true){    // 由于使用非阻塞 IO，读取客户端 buffer，一次读取 buf 大小数据，直到全部读取完毕
                    bzero(&buf, sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));
                    if(bytes_read > 0){
                        printf("来自客户端文件描述符 %d 的消息：%s\n", events[i].data.fd, buf);
                        write(events[i].data.fd, buf, sizeof(buf));
                    } else if(bytes_read == -1 && errno == EINTR){  // 客户端正常中断、继续读取
                        printf("继续读取");
                        continue;
                    } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){// 非阻塞 IO，这个条件表示数据全部读取完毕
                        printf("完成一次读取，错误号: %d\n", errno);
                        break;
                    } else if(bytes_read == 0){  // EOF，客户端断开连接
                        printf("EOF，客户端文件描述符 %d 断开连接\n", events[i].data.fd);
                        close(events[i].data.fd);   // 关闭 socket 会自动将文件描述符从 epoll 树上移除
                        break;
                    }
                }
            } else{         // 其他事件，之后的版本实现
                printf("发生其他事件\n");
            }
        }
    }
    close(sockfd);
    return 0;
}
