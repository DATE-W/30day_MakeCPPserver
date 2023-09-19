#pragma once
#include <sys/epoll.h>

class Epoll; // 前向声明Epoll类，表示Epoll类在这里被引用

class Channel
{
private:
    Epoll *ep; // 指向Epoll实例的指针
    int fd; // 文件描述符
    uint32_t events; // 事件掩码，表示关注的事件
    uint32_t revents; // 触发的事件
    bool inEpoll; // 标志位，表示是否在epoll中

public:
    // 构造函数，需要传入Epoll实例和文件描述符
    Channel(Epoll *_ep, int _fd);

    // 析构函数
    ~Channel();

    // 启用对文件描述符的读事件监视
    void enableReading();

    // 获取文件描述符
    int getFd();

    // 获取关注的事件掩码
    uint32_t getEvents();

    // 获取触发的事件掩码
    uint32_t getRevents();

    // 获取是否在epoll中的标志位
    bool getInEpoll();

    // 设置在epoll中的标志位
    void setInEpoll();

    // 设置触发的事件掩码
    void setRevents(uint32_t);
};
