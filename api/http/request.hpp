#pragma once

#include <string>
#include <vector>
#include <map>
#include <common/api/http/http.hpp>

namespace http {

class Request {
 public:
  enum MethodType {
    GET,
    POST,
    PUT,
    DELETE
  };

  MethodType GetMethod() const;
  std::string GetMethodName() const;
  const std::string& GetUri() const;
  const SSMap& GetHeaders() const;
  bool ExistsPathVariable(const std::string& key) const;
  const SSMap& GetPathVariables() const;
  std::string GetPathVariable(const std::string& key) const;
  bool ExistsParameter(const std::string& key) const;
  const SSMap& GetParameters() const;
  std::string GetParameter(const std::string& key) const;
  const std::string& GetBody() const;

 private:
  MethodType method_;
  std::string uri_;
  std::string body_;
  int http_version_major_;
  int http_version_minor_;
  SSMap headers_;
  SSMap path_variables_;
  SSMap parameters_;

  friend class RequestParser;
  friend class Router;
};

};