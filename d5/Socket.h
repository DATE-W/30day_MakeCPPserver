#pragma once

class InetAddress; // 前向声明 InetAddress 类，表示网络地址

class Socket
{
private:
    int fd; // 套接字文件描述符

public:
    // 默认构造函数，创建一个套接字
    Socket();

    // 构造函数，通过现有的文件描述符构建套接字
    Socket(int);

    // 析构函数，用于关闭套接字
    ~Socket();

    // 将套接字绑定到指定的地址
    void bind(InetAddress*);

    // 监听套接字，等待客户端连接
    void listen();

    // 设置套接字为非阻塞模式
    void setnonblocking();

    // 接受客户端连接并返回客户端的套接字文件描述符
    int accept(InetAddress*);

    // 获取套接字文件描述符
    int getFd();
};
