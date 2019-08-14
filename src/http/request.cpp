#include <common/api/http/request.hpp>

namespace http {

Request::MethodType Request::GetMethod() const {
  return method_;
}

std::string Request::GetMethodName() const {
  switch (method_) {
    case GET:
      return "GET";
    case POST:
      return "POST";
    case DELETE:
      return "DELETE";
    case PUT:
      return "PUT";
    default:
      return "UNKNOWN";
  }
}

const std::string& Request::GetUri() const {
  return uri_;
}

const std::map<std::string, std::string>& Request::GetHeaders() const {
  return headers_;
}

bool Request::ExistsPathVariable(const std::string& key) const {
  return path_variables_.count(key) != 0;
}

const std::map<std::string, std::string>& Request::GetPathVariables() const {
  return path_variables_;
}

std::string Request::GetPathVariable(const std::string& key) const {
  if (!ExistsPathVariable(key)) {
    return "";
  }

  return path_variables_.find(key)->second;
}

bool Request::ExistsParameter(const std::string& key) const {
  return parameters_.count(key) != 0;
}

const std::map<std::string, std::string>& Request::GetParameters() const {
  return parameters_;
}

std::string Request::GetParameter(const std::string& key) const {
  if (!ExistsParameter(key)) {
    return "";
  }

  return parameters_.find(key)->second;
}

const std::string& Request::GetBody() const {
  return body_;
}

};