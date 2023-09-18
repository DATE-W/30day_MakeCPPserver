#include "Epoll.h"
#include "util.h"
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 1000

Epoll::Epoll() : epfd(-1), events(nullptr){
    epfd = epoll_create1(0); // 创建一个 epoll 实例
    errif(epfd == -1, "epoll创建错误"); // 检查是否成功创建 epoll 实例
    events = new epoll_event[MAX_EVENTS]; // 创建事件数组
    bzero(events, sizeof(*events) * MAX_EVENTS); // 初始化事件数组
}

Epoll::~Epoll(){
    if(epfd != -1){
        close(epfd); // 关闭 epoll 实例
        epfd = -1;
    }
    delete [] events; // 释放事件数组的内存
}

// 向 epoll 实例中添加文件描述符和关注的事件
void Epoll::addFd(int fd, uint32_t op){
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll添加事件错误");
}

// 执行 epoll 的等待并获取触发的事件
std::vector<epoll_event> Epoll::poll(int timeout){
    std::vector<epoll_event> activeEvents; // 存储触发的事件
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout); // 等待事件发生
    errif(nfds == -1, "epoll等待错误"); // 检查等待过程是否出错
    for(int i = 0; i < nfds; ++i){
        activeEvents.push_back(events[i]); // 将触发的事件添加到容器中
    }
    return activeEvents; // 返回触发的事件
}
