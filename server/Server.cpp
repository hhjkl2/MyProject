#include "Server.h"
#include <iostream>
#include <string.h>
Server::Server() : m_is_stop{false}
{
    std::cout << "Server is running!" << std::endl;

    // 1.初始化epoll_fd
    m_epoll_fd = epoll_create(5);
    if (m_epoll_fd == -1)
    {
        std::cerr << "epoll creat failed" << std::endl;
        exit(-1);
    }

    // 2.初始化服务器套接字
    m_server_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (m_server_fd < 0)
    {
        std::cerr << "socket creation failed" << std::endl;
        exit(-1);
    }

    // 2.1.设置套接字可复用（仅测试使用）
    setReuseAddr(m_server_fd);

    // 3.绑定服务器套接字
    if (_bind(m_server_fd, "0.0.0.0", 8080))
    {
        close(m_server_fd);
        exit(-1);
    }
}

Server::~Server()
{
    // 关闭服务器套接字
    close(m_server_fd);
    std::cout << "Server is end!" << std::endl;
}

void Server::start()
{
    if (listen(m_server_fd, 5) == -1)
    {
        std::cerr << "socket listen failed" << std::endl;
        close(m_server_fd);
        exit(-1);
    }

    addFd(m_epoll_fd, m_server_fd);
}

void Server::loop()
{
    epoll_event events[MAX_EVENT_NUMBER];

    while (m_is_stop != true)
    {
        int ret = epoll_wait(m_epoll_fd, events, MAX_EVENT_NUMBER, -1);
        if (ret < 0)
        {
            std::cerr << "epoll wait failed" << std::endl;
            break;
        }

        for (int i = 0; i < ret; i++)
        {
            int socket = events[i].data.fd;
            // 如果是服务器socket则一定是监听事件
            if (socket == m_server_fd)
            {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(m_server_fd, (sockaddr *)&client_addr, &client_len);
                if (client_fd != -1)
                {
                    // 将客户端套接字加入epoll中
                    addFd(m_epoll_fd, client_fd);
                }
                else
                {
                    std::cerr << "accept failed" << std::endl;
                }
            }
            else if (events[i].events & EPOLLIN)
            {
            }
            else
            {
                std::cerr << "something else happened" << std::endl;
            }
        }
    }
}

void Server::setReuseAddr(int server_fd)
{
    int reuse = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
}

bool Server::_bind(int server_fd, const char *ip, short port)
{
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    int ret = inet_pton(AF_INET, ip, &server_addr.sin_addr);
    if (ret == 0)
    {
        // 失败：IP格式无效
        std::cerr << "Invalid IP format: " << ip << std::endl;
        return false;
    }
    else if (ret > 0)
    {
        // 系统错误处理
        std::cerr << "inet_pton error" << std::endl;
        std::cerr << "错误码: " << errno << "，错误信息: " << strerror(errno) << std::endl;
        return false;
    }

    if (bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cerr << "socket bind failed" << std::endl;
        std::cerr << "错误码: " << errno << "，错误信息: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

void Server::addFd(int epoll_fd, int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    setNonblocking(fd);
}

int Server::setNonblocking(int fd)
{
    // 设置文件描述符的状态标志 （File Status Flags​​）
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}