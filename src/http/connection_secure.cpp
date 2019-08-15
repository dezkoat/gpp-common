#include <iostream>
#include <iomanip>
#include <common/api/http/connection_manager.hpp>

namespace http {

ConnectionSecure::ConnectionSecure(
  asio::ip::tcp::socket socket,
  asio::ssl::context& context,
  ConnectionManager& manager,
  std::map<const std::string, std::shared_ptr<Router>>& routers
) :
  ConnectionBase(manager, routers),
  socket_(std::move(socket), context)
{
}

void ConnectionSecure::Start() {
  DoHandshake();
}

void ConnectionSecure::Stop() {
  socket_.lowest_layer().close();
}

void ConnectionSecure::DoRead() {
  auto self(shared_from_this());
  socket_.async_read_some(
    asio::buffer(buffer_),
    std::bind(
      &ConnectionSecure::ReadSome,
      shared_from_this(),
      std::placeholders::_1,
      std::placeholders::_2
    )
  );
}

void ConnectionSecure::DoWrite() {
  auto self(shared_from_this());
  asio::async_write(
    socket_,
    response_.ToBuffers(),
    [this, self](const std::error_code& ec, std::size_t) {
      if (!ec) {
        asio::error_code ignored_ec;
        socket_.lowest_layer().shutdown(
          asio::ip::tcp::socket::shutdown_both,
          ignored_ec
        );
      }

      if ((ec.value() & 0xFF) == SSL_R_HTTP_REQUEST
          && ec != asio::error::operation_aborted) {
        connection_manager_.Stop(shared_from_this());
      }
    }
  );
}

void ConnectionSecure::DoHandshake() {
  auto self(shared_from_this());

  socket_.async_handshake(asio::ssl::stream_base::server,
    [this, self](const std::error_code& ec) {
      DoRead();
    }
  );
}

};