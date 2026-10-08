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

  class TcpServer
  {
  public:
    explicit TcpServer();
    ~TcpServer();

    int startServer();
    void closeServer();

  private:
    int m_socket = -1;
    int m_new_socket = -1;
  };
} // namespace http
