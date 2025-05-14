#include <sys/epoll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024
class Server
{
private:
    int m_epoll_fd;
    int m_server_fd;
    bool m_is_stop;

public:
    Server();
    ~Server();

public:
    // 服务器开始监听，并加入epoll集合中
    void start();

    // epoll_waite循环处理事件
    void loop();

private:
    // 设置地址可以立即使用
    void setReuseAddr(int server_fd);
    bool _bind(int server_fd, const char *ip, short port);
    void addFd(int epoll_fd, int fd);
    int setNonblocking(int fd);
};
