#pragma once

#include <asio.hpp>
#include <map>
#include <common/api/http/response.hpp>
#include <common/api/http/request.hpp>

#define ROUTE_END_POINT(s) s

namespace http {

class Router {
 public:
  typedef void (Router::*RouterMethod)(const Request&, Response&);
  typedef std::pair<std::string, Request::MethodType> PairKey;
  typedef std::pair<const PairKey, RouterMethod> PairMethod;

  void SetRoute(std::string);

 protected:
  template<typename T>
  void AddEndpoint(
    std::string endpoint,
    Request::MethodType method_type,
    T&& method
  ) {
    AddEndpoint(endpoint, method_type, reinterpret_cast<RouterMethod>(method));
  }

 private:
  std::map<PairKey, RouterMethod> endpoints_static_;
  std::vector<PairMethod> endpoints_dynamic_;
  std::string route_;

  void HandleRequest(Request&, Response&);
  void AddEndpoint(
    std::string endpoint,
    Request::MethodType method_type,
    RouterMethod method
  );
  friend class ConnectionBase;
};

};