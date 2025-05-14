// 服务器代码
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

int main()
{
  std::cout << "Server is running!" << std::endl;

  // 创建TCP套接字
  int server_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
  {
    std::cerr << "socket creation failed" << std::endl;
    return -1;
  }

  // 设置套接字为可重用
  int reuse = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

  // 绑定套接字
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  // 监听任何来源地址
  inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);
  bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr));

  // 监听客户端
  listen(server_fd, 5);

  // 接收客户端连接
  struct sockaddr_in client_addr;
  socklen_t len = sizeof(client_addr);
  memset(&client_addr, 0, sizeof(client_addr));
  int client_fd = accept(server_fd, (sockaddr *)&client_addr, &len);
  if (client_fd < 0)
  {
    std::cerr << "socket accept failed" << std::endl;
    return -1;
  }
  else
  {
    char buffer[1024];
    // 提取客户端 IP 地址
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

    // 提取客户端端口号（需转换字节序）
    uint16_t client_port = ntohs(client_addr.sin_port);

    std::cout << "accept recv from IP: " << client_ip
              << " Port: " << client_port << std::endl;

    ssize_t bytes_read = recv(client_fd, buffer, 1024, 0);
    if (bytes_read < 0)
    {
      std::cerr << "Read error" << std::endl;
      return -1;
    }

    // 打印原始 HTTP 请求内容
    std::cout << "Received raw HTTP request:\n"
              << buffer << std::endl;

    // 发送简单响应（非完整 HTTP 协议实现）
    const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
    send(client_fd, response, strlen(response), 0);
    close(client_fd);
  }

  // 关闭服务器连接
  close(server_fd);
  std::cout << "Server is end!" << std::endl;
  return 0;
}