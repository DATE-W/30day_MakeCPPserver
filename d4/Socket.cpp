#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

// 默认构造函数，创建一个套接字
Socket::Socket() : fd(-1){
    fd = socket(AF_INET, SOCK_STREAM, 0); // 创建一个IPv4 TCP套接字
    errif(fd == -1, "socket创建错误"); // 检查是否成功创建套接字
}

// 构造函数，通过现有的文件描述符构建套接字
Socket::Socket(int _fd) : fd(_fd){
    errif(fd == -1, "socket创建错误"); // 检查套接字是否有效
}

// 析构函数，用于关闭套接字
Socket::~Socket(){
    if(fd != -1){
        close(fd); // 关闭套接字
        fd = -1;
    }
}

// 将套接字绑定到指定的地址
void Socket::bind(InetAddress *addr){
    errif(::bind(fd, (sockaddr*)&addr->addr, addr->addr_len) == -1, "socket绑定错误");
    // 使用给定的地址结构将套接字绑定到指定的IP地址和端口上，并检查绑定是否成功
}

// 监听套接字，等待客户端连接
void Socket::listen(){
    errif(::listen(fd, SOMAXCONN) == -1, "socket监听错误");
    // 启动套接字监听模式，并设置最大连接队列长度为系统允许的最大值，检查是否成功启动监听
}

// 设置套接字为非阻塞模式
void Socket::setnonblocking(){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
    // 使用fcntl函数将套接字设置为非阻塞模式，允许非阻塞的读写操作
}

// 接受客户端连接并返回客户端的套接字文件描述符
int Socket::accept(InetAddress *addr){
    int clnt_sockfd = ::accept(fd, (sockaddr*)&addr->addr, &addr->addr_len);
    errif(clnt_sockfd == -1, "socket接受连接错误");
    // 使用accept函数接受客户端的连接请求，并返回客户端的套接字文件描述符，同时检查是否成功接受连接
    return clnt_sockfd;
}

// 获取套接字文件描述符
int Socket::getFd(){
    return fd; // 返回套接字文件描述符
}
