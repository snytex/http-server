#pragma once

#include <unordered_map>
#include <string_view>
#include <string>

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

  std::string_view methodToString(Method method);

  struct Request
  {
    Method method = Method::INVALID;
    std::string path;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
  };

  class HttpParser
  {
  public:
    explicit HttpParser();
    ~HttpParser();

    Request parseRequestLine(std::string_view line);

    std::unordered_map<std::string, std::string>
    parseHeaderLine(std::string_view line);

    Request parseRequest(std::string_view data);

    void printRequest(Request& request);

  private:
  };
} // namespace http
