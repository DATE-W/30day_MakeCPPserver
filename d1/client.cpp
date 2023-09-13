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
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器的IP地址
    serv_addr.sin_port = htons(8888); // 服务器的端口号

    // 连接到服务器
    connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // 在这里可以使用 sockfd 进行数据传输，与服务器通信

    return 0;
}
