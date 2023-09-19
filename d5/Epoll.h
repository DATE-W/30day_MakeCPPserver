#pragma once
#include <sys/epoll.h>
#include <vector>
class Channel;
class Epoll
{
private:
    int epfd; // epoll 实例的文件描述符
    struct epoll_event *events; // 事件数组指针

public:
    // 构造函数，创建 epoll 实例和事件数组
    Epoll();

    // 析构函数，释放资源
    ~Epoll();

    // 向 epoll 实例中添加文件描述符和关注的事件
    void addFd(int fd, uint32_t op);

    // 修改 epoll 实例
    void updateChannel(Channel*);

    // 执行 epoll 的等待并获取触发的事件
    std::vector<Channel*> poll(int timeout = -1);
};
