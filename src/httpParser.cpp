#include "httpParser.h"
#include <print>

namespace http
{
  static const std::unordered_map<std::string_view, Method> methodMap = {
      {"GET", Method::GET},       {"QUERY", Method::QUERY},
      {"HEAD", Method::HEAD},     {"OPTIONS", Method::OPTIONS},
      {"TRACE", Method::TRACE},   {"PUT", Method::PUT},
      {"DELETE", Method::DELETE}, {"POST", Method::POST},
      {"PATCH", Method::PATCH},   {"CONNECT", Method::CONNECT},
  };

  std::string_view methodToString(Method method)
  {
    switch (method)
    {
    case Method::GET:
      return std::string_view("GET");

    case Method::HEAD:
      return std::string_view("HEAD");

    case Method::TRACE:
      return std::string_view("TRACE");

    case Method::DELETE:
      return std::string_view("DELETE");

    case Method::PATCH:
      return std::string_view("PATCH");

    case Method::QUERY:
      return std::string_view("QUERY");

    case Method::OPTIONS:
      return std::string_view("OPTIONS");

    case Method::PUT:
      return std::string_view("PUT");

    case Method::POST:
      return std::string_view("POST");

    case Method::CONNECT:
      return std::string_view("CONNECT");

    default:
      return std::string_view("INVALID");
    }
  }

  HttpParser::HttpParser() {}
  HttpParser::~HttpParser() {}

  Request HttpParser::parseRequestLine(std::string_view line)
  {
    auto firstSpace = line.find(' ');
    if (firstSpace == std::string_view::npos)
      return {};

    auto secondSpace = line.find(' ', firstSpace + 1);
    if (secondSpace == std::string_view::npos)
      return {};

    auto methodText = line.substr(0, firstSpace);
    auto pathText = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    auto versionText = line.substr(secondSpace + 1);

    Method method{Method::INVALID};

    auto it = methodMap.find(methodText);
    if (it != methodMap.end())
      method = it->second;

    const std::string path{pathText};
    const std::string version{versionText};

    return Request{.method = method, .path = path, .version = version};
  }

  std::unordered_map<std::string, std::string>
  HttpParser::parseHeaderLine(std::string_view line)
  {
    auto leader = line.find(':');
    if (leader == std::string_view::npos)
      return {};

    auto nameText = line.substr(0, leader);
    auto valueText = line.substr(leader + 1);

    if (!valueText.empty() && valueText.front() == ' ')
      valueText = valueText.substr(1);

    std::string name(nameText);
    std::string value(valueText);

    return std::unordered_map<std::string, std::string>{{name, value}};
  }

  Request HttpParser::parseRequest(std::string_view data)
  {
    auto current = 0;
    auto end = data.find("\r\n", 0);
    std::string requestLine(data.substr(0, end));

    Request r = parseRequestLine(requestLine);

    while (true)
    {
      current = (end + 2);
      end = data.find("\r\n", current);

      if (end == std::string_view::npos || end == current)
        break;

      auto leader = data.find(":", current);
      if (leader == std::string_view::npos || leader > end)
        break;

      std::string name(data.substr(current, leader - current));
      std::string value(data.substr(leader + 1, end - (leader + 1)));

      // remove empty space
      if (!value.empty() && value.front() == ' ')
        value = value.substr(1);

      r.headers.emplace(name, value);
    }

    return r;
  }

  void HttpParser::printRequest(Request& request)
  {
    std::println("Method: {}", methodToString(request.method));
    std::println("Path: {}", request.path);
    std::println("Version: {}", request.version);

    std::println();
    for (const auto& pair : request.headers)
    {
      std::println("{} = {}", pair.first, pair.second);
    }
  }
} // namespace http
