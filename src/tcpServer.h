#pragma once
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <unordered_map>

/*
                              /\
                            _/./
            fish        ,-'    `-:.,-'/
            ---->      > O )<)    _  (
                         `-._  _.:' `-.\
                             `` \;
*/

namespace http
{
  enum class Method
  {
    GET = 0,
    QUERY,
    HEAD,
    OPTIONS,
    TRACE,
    PUT,
    DELETE,
    POST,
    PATCH,
    CONNECT,
    INVALID,
  };

  struct Request
  {
    Method method = Method::INVALID;
    std::string path;
    std::string version;
  };

  class TcpServer
  {
  public:
    explicit TcpServer();
    ~TcpServer();

    int startServer();
    void closeServer();

    Request parseRequestLine(std::string_view line);
    std::unordered_map<std::string, std::string>
    parseHeaderLine(std::string_view line);
    // I hate when my formatter does this to my functions

  private:
    int m_socket = -1;
    int m_new_socket = -1;
  };
} // namespace http
