#pragma once

#include <asio/ssl.hpp>
#include <common/api/http/connection.hpp>

namespace http {

class ConnectionManager;

class ConnectionSecure : public ConnectionBase {
 public:
  ConnectionSecure(const ConnectionSecure&) = delete;
  ConnectionSecure& operator=(const ConnectionSecure&) = delete;
  explicit ConnectionSecure(
    asio::ip::tcp::socket socket,
    asio::ssl::context& context,
    ConnectionManager& manager,
    std::map<const std::string, std::shared_ptr<Router>>& routers
  );
  void Start();
  void Stop();

 private:
  void DoRead();
  void DoWrite();
  void DoHandshake();

  asio::ssl::stream<asio::ip::tcp::socket> socket_;
};

};