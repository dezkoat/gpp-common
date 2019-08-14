#include <common/api/http/response.hpp>

namespace http {

namespace status {
  
const std::string OK =
  "HTTP/1.0 200 OK\r\n";
const std::string CREATED =
  "HTTP/1.0 201 Created\r\n";
const std::string ACCEPTED =
  "HTTP/1.0 202 Accepted\r\n";
const std::string NO_CONTENT =
  "HTTP/1.0 204 No Content\r\n";
const std::string MULTIPLE_CHOICES =
  "HTTP/1.0 300 Multiple Choices\r\n";
const std::string MOVED_PERMANENTLY =
  "HTTP/1.0 301 Moved Permanently\r\n";
const std::string MOVED_TEMPORARILY =
  "HTTP/1.0 302 Moved Temporarily\r\n";
const std::string NOT_MODIFIED =
  "HTTP/1.0 304 Not Modified\r\n";
const std::string BAD_REQUEST =
  "HTTP/1.0 400 Bad Request\r\n";
const std::string UNAUTHORIZED =
  "HTTP/1.0 401 Unauthorized\r\n";
const std::string FORBIDDEN =
  "HTTP/1.0 403 Forbidden\r\n";
const std::string NOT_FOUND =
  "HTTP/1.0 404 Not Found\r\n";
const std::string INTERNAL_SERVER_ERROR =
  "HTTP/1.0 500 Internal Server Error\r\n";
const std::string NOT_IMPLEMENTED =
  "HTTP/1.0 501 Not Implemented\r\n";
const std::string BAD_GATEWAY =
  "HTTP/1.0 502 Bad Gateway\r\n";
const std::string SERVICE_UNAVAILABLE =
  "HTTP/1.0 503 Service Unavailable\r\n";

#define STATUS_BUFFER(status) case Response::status: return asio::buffer(status)
asio::const_buffer ToBuffer(Response::StatusType status) {
  switch (status) {
    STATUS_BUFFER(OK);
    STATUS_BUFFER(CREATED);
    STATUS_BUFFER(ACCEPTED);
    STATUS_BUFFER(NO_CONTENT);
    STATUS_BUFFER(MULTIPLE_CHOICES);
    STATUS_BUFFER(MOVED_PERMANENTLY);
    STATUS_BUFFER(MOVED_TEMPORARILY);
    STATUS_BUFFER(NOT_MODIFIED);
    STATUS_BUFFER(BAD_REQUEST);
    STATUS_BUFFER(UNAUTHORIZED);
    STATUS_BUFFER(FORBIDDEN);
    STATUS_BUFFER(NOT_FOUND);
    STATUS_BUFFER(INTERNAL_SERVER_ERROR);
    STATUS_BUFFER(NOT_IMPLEMENTED);
    STATUS_BUFFER(BAD_GATEWAY);
    STATUS_BUFFER(SERVICE_UNAVAILABLE);
    default:
      return asio::buffer(INTERNAL_SERVER_ERROR);
  }
}

};

const std::string crlf = "\r\n";
const std::string sep = ": ";

std::vector<asio::const_buffer> Response::ToBuffers() {
  std::vector<asio::const_buffer> buffers;
  buffers.push_back(status::ToBuffer(status_));
  for (auto& it : headers_) {
    buffers.push_back(asio::buffer(it.first));
    buffers.push_back(asio::buffer(sep));
    buffers.push_back(asio::buffer(it.second));
    buffers.push_back(asio::buffer(crlf));
  }
  buffers.push_back(asio::buffer(crlf));
  buffers.push_back(asio::buffer(content_));
  return buffers;
}

#define GET_STRING(v) case Response::v: return status::v
std::string DefaultContent(Response::StatusType status) {
  switch (status) {
    GET_STRING(OK);
    GET_STRING(CREATED);
    GET_STRING(ACCEPTED);
    GET_STRING(NO_CONTENT);
    GET_STRING(MULTIPLE_CHOICES);
    GET_STRING(MOVED_PERMANENTLY);
    GET_STRING(MOVED_TEMPORARILY);
    GET_STRING(NOT_MODIFIED);
    GET_STRING(BAD_REQUEST);
    GET_STRING(UNAUTHORIZED);
    GET_STRING(FORBIDDEN);
    GET_STRING(NOT_FOUND);
    GET_STRING(INTERNAL_SERVER_ERROR);
    GET_STRING(NOT_IMPLEMENTED);
    GET_STRING(BAD_GATEWAY);
    GET_STRING(SERVICE_UNAVAILABLE);
    default:
      return status::INTERNAL_SERVER_ERROR;
  }
}

void Response::SetContent(const std::string& content) {
  SetContent(content, "text/plain");
}

void Response::SetContent(
  const std::string& content,
  const std::string& content_type
) {
  auto content_length = std::to_string(content.length());
  content_ = content;
  headers_.insert(SSPair("Content-Length", content_length));
  headers_.insert(SSPair("Content-Type", content_type));
}

void Response::AddHeader(
  const std::string& key,
  const std::string& value
) {
  headers_.insert(SSPair(key, value));
}

void Response::SetStatus(Response::StatusType status) {
  status_ = status;
}

Response Response::Generate(const StatusType& status) {
  Response res;
  auto content_length = std::to_string(res.content_.length());
  res.status_ = status;
  res.content_ = DefaultContent(status);
  res.headers_.insert(SSPair("Content-Length", content_length));
  res.headers_.insert(SSPair("Content-Type", "text/html"));
  return res;
}

};