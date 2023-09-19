#include "Epoll.h"
#include "util.h"
#include "Channel.h"
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
std::vector<Channel*> Epoll::poll(int timeout){
    std::vector<Channel*> activeChannels;
    
    // 使用 epoll_wait 等待事件的触发，返回触发事件的文件描述符数量
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    
    // 检查是否出现错误
    errif(nfds == -1, "epoll wait error");
    
    for(int i = 0; i < nfds; ++i){
        // 获取触发事件的对应的 Channel 对象指针
        Channel *ch = (Channel*)events[i].data.ptr;
        
        // 设置触发的事件类型（如EPOLLIN等）到 Channel 对象的 revents 属性
        ch->setRevents(events[i].events);
        
        // 将触发事件的 Channel 添加到活跃 Channel 列表中
        activeChannels.push_back(ch);
    }
    
    // 返回包含触发事件的 Channel 对象的向量
    return activeChannels;
}


void Epoll::updateChannel(Channel *channel) {
    int fd = channel->getFd();
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));

    // 设置事件结构体中的指针，通常用于关联事件与对应的Channel对象
    ev.data.ptr = channel;
    ev.events = channel->getEvents();

    // 检查Channel是否已经在Epoll中注册
    if (!channel->getInEpoll()) {
        // 如果未在Epoll中注册，则尝试添加新的事件
        errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
        // 设置Channel标志，表示已经在Epoll中注册
        channel->setInEpoll();
    } else {
        // 如果已经在Epoll中注册，则尝试修改事件
        errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");
    }
}
