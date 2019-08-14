#include <common/api/http/connection_manager.hpp>

namespace http {

ConnectionManager::ConnectionManager() {
}

void ConnectionManager::Start(ConnectionPtr c) {
  connections_.insert(c);
  c->Start();
}

void ConnectionManager::Stop(ConnectionPtr c) {
  connections_.erase(c);
  c->Stop();
}

void ConnectionManager::StopAll() {
  for (auto c : connections_)
    c->Stop();
  connections_.clear();
}

};