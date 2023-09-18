#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "util.h"

#define BUFFER_SIZE 1024 // 缓冲区大小

int main() {
    // 创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket创建失败");

    // 配置服务器地址
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器的IP地址
    serv_addr.sin_port = htons(8888); // 服务器的端口号

    // 连接到服务器
    errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "连接服务器失败");
    
    while(true){
        char buf[BUFFER_SIZE];
        bzero(&buf, sizeof(buf));

        // 用户输入消息
        scanf("%s", buf);

        // 发送消息到服务器
        ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
        if(write_bytes == -1){
            printf("套接字已断开连接，无法继续写入数据！\n");
            break;
        }

        bzero(&buf, sizeof(buf));

        // 从服务器读取响应
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if(read_bytes > 0){
            printf("来自服务器的消息: %s\n", buf);
        }else if(read_bytes == 0){
            printf("服务器套接字断开连接!\n");
            break;
        }else if(read_bytes == -1){
            close(sockfd);
            errif(true, "套接字读取错误");
        }
    }
    
    // 关闭套接字
    close(sockfd);
    return 0;
}
