#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

int main() {
    // 创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 配置服务器地址
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器监听的IP地址
    serv_addr.sin_port = htons(8888); // 服务器监听的端口号

    // 将套接字绑定到服务器地址
    bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // 监听连接请求
    listen(sockfd, SOMAXCONN); // SOMAXCONN 指定连接队列的最大长度
    
    // 接受客户端连接请求
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    bzero(&clnt_addr, sizeof(clnt_addr));

    int clnt_sockfd = accept(sockfd, (struct sockaddr*)&clnt_addr, &clnt_addr_len);

    // 打印客户端的IP地址和端口号
    printf("新客户端套接字文件描述符：%d，IP地址：%s，端口号：%d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

    return 0;
}
