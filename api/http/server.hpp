#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <string>
#include <map>
#include <common/api/http/connection_manager.hpp>
#include <common/api/http/router.hpp>

namespace http {

class Server {
 public:
  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;
  explicit Server(
    const std::string& address,
    const std::string& port,
    bool secure = false
  );
  void Run();
  void Route(const std::string, std::shared_ptr<Router>);

 private:
  void DoAccept();
  void DoAwaitStop();
  std::string GetPassword() const;

  asio::io_context io_context_;
  asio::ssl::context ssl_context_;
  asio::signal_set signals_;
  asio::ip::tcp::acceptor acceptor_;
  ConnectionManager connection_manager_;
  std::map<const std::string, std::shared_ptr<Router>> routers_;
  bool secure_;
};

};