#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include "util.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

// 处理可读事件的函数
void handleReadEvent(int);

int main() {
    // 创建服务器Socket对象
    Socket *serv_sock = new Socket();
    
    // 创建服务器地址对象
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    
    // 将Socket对象绑定到服务器地址
    serv_sock->bind(serv_addr);
    
    // 启动Socket监听
    serv_sock->listen();    
    
    // 创建Epoll对象
    Epoll *ep = new Epoll();
    
    // 将服务器Socket设置为非阻塞模式
    serv_sock->setnonblocking();
    
    // 创建服务器Channel对象
    Channel *servChannel = new Channel(ep, serv_sock->getFd());
    
    // 启用对服务器Socket的读事件监视
    servChannel->enableReading();
    
    while(true){
        // 通过Epoll对象获取活跃的Channel列表
        std::vector<Channel*> activeChannels = ep->poll();
        int nfds = activeChannels.size();
        
        for(int i = 0; i < nfds; ++i){
            int chfd = activeChannels[i]->getFd();
            
            if(chfd == serv_sock->getFd()){        // 新客户端连接
                InetAddress *clnt_addr = new InetAddress();
                
                // 接受新的客户端连接，并创建客户端Socket对象
                Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr));
                
                // 打印客户端连接信息
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
                
                // 将客户端Socket设置为非阻塞模式
                clnt_sock->setnonblocking();
                
                // 创建客户端Channel对象
                Channel *clntChannel = new Channel(ep, clnt_sock->getFd());
                
                // 启用对客户端Socket的读事件监视
                clntChannel->enableReading();
            } else if(activeChannels[i]->getRevents() & EPOLLIN){      // 可读事件
                handleReadEvent(activeChannels[i]->getFd());
            } else{         // 其他事件，之后的版本实现
                printf("something else happened\n");
            }
        }
    }
    
    // 清理内存
    delete serv_sock;
    delete serv_addr;
    
    return 0;
}

// 处理可读事件的函数实现
void handleReadEvent(int sockfd){
    char buf[READ_BUFFER];
    
    while(true){    // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        
        if(bytes_read > 0){
            // 打印来自客户端的消息
            printf("message from client fd %d: %s\n", sockfd, buf);
            
            // 将消息回传给客户端
            write(sockfd, buf, sizeof(buf));
        } else if(bytes_read == -1 && errno == EINTR){  // 客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){ // 非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if(bytes_read == 0){  // EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            
            // 关闭Socket会自动将文件描述符从epoll树上移除
            close(sockfd);
            break;
        }
    }
}
