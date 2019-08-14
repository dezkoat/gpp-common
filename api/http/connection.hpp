#pragma once

#include <asio.hpp>
#include <string>
#include <map>
#include <common/api/http/response.hpp>
#include <common/api/http/request.hpp>
#include <common/api/http/request_parser.hpp>
#include <common/api/http/router.hpp>

namespace http {

class ConnectionManager;

class ConnectionBase : public std::enable_shared_from_this<ConnectionBase> {
 public:
  ConnectionBase(
    ConnectionManager& manager,
    std::map<const std::string, std::shared_ptr<Router>>& routers
  );
  virtual void Start() {};
  virtual void Stop() {};

 protected:
  ConnectionManager& connection_manager_;
  std::map<const std::string, std::shared_ptr<Router>>& routers_;
  std::array<char, 8192> buffer_;
  Request request_;
  RequestParser request_parser_;
  Response response_;
};

class Connection : public ConnectionBase {
 public:
  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;
  explicit Connection(
    asio::ip::tcp::socket socket,
    ConnectionManager& manager,
    std::map<const std::string, std::shared_ptr<Router>>& routers
  );
  void Start();
  void Stop();

 private:
  void DoRead();
  void DoWrite();

  asio::ip::tcp::socket socket_;
};

typedef std::shared_ptr<ConnectionBase> ConnectionPtr;

};