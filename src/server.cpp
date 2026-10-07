#include "tcpServer.h"

// good watch: https://www.youtube.com/watch?v=4UYYzbzGk6s

int main(void)
{
  using namespace http;

  TcpServer server = TcpServer();
  server.startServer();

  return 0;
}
