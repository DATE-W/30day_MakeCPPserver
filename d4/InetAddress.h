#pragma once
#include <arpa/inet.h>

class InetAddress
{
public:
    struct sockaddr_in addr; // 用于存储IPv4地址信息的结构体
    socklen_t addr_len; // 地址结构体的长度

    // 默认构造函数，初始化地址结构体和长度
    InetAddress();

    // 构造函数，根据给定的IP地址和端口号创建地址结构体
    InetAddress(const char* ip, uint16_t port);

    // 析构函数，无特殊操作
    ~InetAddress();
};
