#include <iostream>
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
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器的IP地址
    serv_addr.sin_port = htons(8888); // 服务器的端口号

    // 连接到服务器
    errif(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");
    
    while(true){
        char buf[1024];
        bzero(&buf, sizeof(buf));
        scanf("%s", buf); // 从用户输入读取消息
        ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
        if(write_bytes == -1){
            printf("套接字已断开连接，无法继续写入数据！\n");
            break;
        }
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if(read_bytes > 0){
            printf("来自服务器的消息: %s\n", buf); // 打印服务器的响应
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
