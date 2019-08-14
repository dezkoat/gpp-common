#pragma once

#include <asio.hpp>
#include <set>
#include <common/api/http/connection.hpp>
#include <common/api/http/connection_secure.hpp>

namespace http {

class ConnectionManager {
 public:
  ConnectionManager(const ConnectionManager&) = delete;
  ConnectionManager& operator=(const ConnectionManager&) = delete;
  ConnectionManager();

  void Start(ConnectionPtr c);
  void Stop(ConnectionPtr c);
  void StopAll();

 private:
  std::set<ConnectionPtr> connections_;
};

};