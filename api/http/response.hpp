#pragma once

#include <asio.hpp>
#include <string>
#include <vector>
#include <common/api/http/http.hpp>

namespace http {

class Response {
 public:

  enum StatusType {
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,
    MULTIPLE_CHOICES = 300,
    MOVED_PERMANENTLY = 301,
    MOVED_TEMPORARILY = 302,
    PERMANENT_REDIRECT = 308,
    NOT_MODIFIED = 304,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503
  };

  void SetStatus(StatusType);
  void SetContent(const std::string&);
  void SetContent(const std::string&, const std::string&);
  void AddHeader(const std::string&, const std::string&);
  static Response Generate(const StatusType&);

  inline const std::string GetContent() const { return content_; }

 public:
  StatusType status_ = StatusType::OK; // Default response is Ok

  SSMap headers_;
  std::string content_;

  std::vector<asio::const_buffer> ToBuffers();

  friend class Connection;
};

};