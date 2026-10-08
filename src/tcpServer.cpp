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
      if (sent < 0)
        std::print(stderr, "[ERROR] Cannot reply to client");
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

  void TcpServer::closeServer()
  {
    close(m_socket);
    close(m_new_socket);
  }
} // namespace http
