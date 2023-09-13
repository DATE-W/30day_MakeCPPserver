#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "util.h"

int main() {
    // 创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    // 配置服务器地址
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器监听的IP地址
    serv_addr.sin_port = htons(8888); // 服务器监听的端口号

    // 绑定套接字到服务器地址
    errif(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");

    // 监听连接请求
    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");
    
    // 接受客户端连接请求
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    bzero(&clnt_addr, sizeof(clnt_addr));

    int clnt_sockfd = accept(sockfd, (struct sockaddr*)&clnt_addr, &clnt_addr_len);
    errif(clnt_sockfd == -1, "socket accept error");

    // 打印客户端连接信息
    printf("新客户端套接字文件描述符：%d，IP地址：%s，端口号：%d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

    while (true) {
        char buf[1024];
        bzero(&buf, sizeof(buf));
        
        // 从客户端读取数据
        ssize_t read_bytes = read(clnt_sockfd, buf, sizeof(buf));
        if(read_bytes > 0){
            // 打印接收到的消息并将其发送回客户端
            printf("来自客户端文件描述符 %d 的消息：%s\n", clnt_sockfd, buf);
            write(clnt_sockfd, buf, sizeof(buf));
        } else if(read_bytes == 0){
            // 客户端断开连接
            printf("客户端文件描述符 %d 断开连接\n", clnt_sockfd);
            close(clnt_sockfd);
            break;
        } else if(read_bytes == -1){
            // 读取错误，关闭连接
            close(clnt_sockfd);
            errif(true, "socket read error");
        }
    }
    
    // 关闭服务器套接字
    close(sockfd);
    return 0;
}
