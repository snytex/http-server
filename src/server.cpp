#include "tcpServer.h"
#include "httpParser.h"
#include <print>

// good watch: https://www.youtube.com/watch?v=4UYYzbzGk6s

int main(void)
{
  using namespace http;

  char example[256] =
      "GET /test HTTP/1.1\r\nHost: 127.0.0.1:8000\r\nUser-Agent: "
      "curl/...\r\nAccept: */*\r\nContent-Length: 28\r\n\r\n{this is some json "
      "probably}";

  TcpServer server = TcpServer();
  HttpParser parser = HttpParser();

  Request req = parser.parseRequest(std::string_view(example));
  parser.printRequest(req);

  return 0;
}
