#include "tcpServer.h"
#include <stdexcept>
#include <unistd.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <print>
#include <unordered_map>

namespace http
{
  TcpServer::TcpServer() {}
  TcpServer::~TcpServer() { closeServer(); }

  int TcpServer::startServer()
  {
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0)
    {
      std::println(stderr, "[ERROR] Cannot create socket");
      return 1;
    }

    const int enable = 1;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    const char* ipAddress = "127.0.0.1";
    int16_t port = 8080;

    sockaddr_in serverAddress{
        .sin_family = AF_INET, .sin_port = htons(port), .sin_addr = {}};

    // Presentation (human-readable)
    // Numeric (binary/network byte order format)
    int addr = inet_pton(AF_INET, ipAddress, &serverAddress.sin_addr);
    if (addr != 1)
    {
      std::println(stderr, "[ERROR] Cannot create server address: {}",
                   strerror(errno));
      return 1;
    }

    int bindResult = bind(m_socket, reinterpret_cast<sockaddr*>(&serverAddress),
                          sizeof(serverAddress));
    if (bindResult != 0)
    {
      std::println(stderr, "[ERROR] Cannot bind server address: {}",
                   strerror(errno));
      return 1;
    }

    int listenResult = listen(m_socket, 4);
    if (listenResult != 0)
    {
      std::println(stderr, "[ERROR] Cannot listen to socket: {}",
                   strerror(errno));
      return 1;
    }

    std::println(stderr, "[INFO] Server listening on {}:{}", ipAddress, port);

    sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);

    m_new_socket = accept(m_socket, reinterpret_cast<sockaddr*>(&clientAddress),
                          &clientAddressLength);
    if (m_new_socket < 0)
    {
      std::println(stderr, "[ERROR] Cannot accept client: {}", strerror(errno));
      return 1;
    }

    std::println(stdout, "[INFO] Client accepted");

    char buf[4096];

    ssize_t received = recv(m_new_socket, buf, sizeof(buf), 0);
    if (received > 0)
    {
      // print the received buffer
      for (ssize_t i = 0; i < received; ++i)
        std::print("{}", buf[i]);
      std::println();

      char reply[53] =
          "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello client!";

      ssize_t sent = send(m_new_socket, reply, sizeof(reply), 0);
    }
    else if (received == 0)
    {
      std::println(stdout, "[INFO] Connection closed by client");
    }
    else // < 0
    {
      std::println(stderr, "[ERROR] Cannot receive packets from client");
      return 1;
    }

    return 0;
  }

  Request TcpServer::parseRequestLine(std::string_view line)
  {
    auto firstSpace = line.find(' ');
    if (firstSpace == line.npos)
      return {};

    auto secondSpace = line.find(' ', firstSpace + 1);
    if (secondSpace == line.npos)
      return {};

    auto methodText = line.substr(0, firstSpace);
    auto pathText = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    auto versionText = line.substr(secondSpace + 1);

    Method method{Method::INVALID};
    static const std::unordered_map<std::string_view, Method> methodMap = {
        {"GET", Method::GET},       {"QUERY", Method::QUERY},
        {"HEAD", Method::HEAD},     {"OPTIONS", Method::OPTIONS},
        {"TRACE", Method::TRACE},   {"PUT", Method::PUT},
        {"DELETE", Method::DELETE}, {"POST", Method::POST},
        {"PATCH", Method::PATCH},   {"CONNECT", Method::CONNECT},
    };

    auto it = methodMap.find(methodText);
    if (it != methodMap.end())
      method = it->second;

    const std::string path{pathText};
    const std::string version{versionText};

    return Request{.method = method, .path = path, .version = version};
  }

  std::unordered_map<std::string, std::string>
  TcpServer::parseHeaderLine(std::string_view line)
  {
    auto leader = line.find(':');
    if (leader == line.npos)
      return {};

    auto nameText = line.substr(0, leader);
    auto valueText = line.substr(leader + 1);

    if (!valueText.empty() && valueText.front() == ' ')
      valueText = valueText.substr(1);

    std::string name(nameText);
    std::string value(valueText);

    return std::unordered_map<std::string, std::string>{{name, value}};
  }

  void TcpServer::closeServer()
  {
    close(m_socket);
    close(m_new_socket);
  }
} // namespace http
