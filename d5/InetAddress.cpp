#include "InetAddress.h"
#include <string.h>

// 默认构造函数，初始化地址结构体和长度
InetAddress::InetAddress() : addr_len(sizeof(addr)){
    bzero(&addr, sizeof(addr)); // 清零地址结构体
}

// 构造函数，根据给定的IP地址和端口号创建地址结构体
InetAddress::InetAddress(const char* ip, uint16_t port) : addr_len(sizeof(addr)){
    bzero(&addr, sizeof(addr)); // 清零地址结构体
    addr.sin_family = AF_INET; // 设置地址族为IPv4
    addr.sin_addr.s_addr = inet_addr(ip); // 设置IP地址
    addr.sin_port = htons(port); // 设置端口号，并进行字节序转换
    addr_len = sizeof(addr); // 设置地址结构体的长度
}

// 析构函数，无特殊操作
InetAddress::~InetAddress(){
}
