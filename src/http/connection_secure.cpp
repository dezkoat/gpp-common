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
    [this, self](const std::error_code& ec, std::size_t bytes_transferred) {
      if (ec) {
        if (ec != asio::error::operation_aborted)
          connection_manager_.Stop(shared_from_this());
        return;
      }

      *(buffer_.data() + bytes_transferred) = 0;
      RequestParser::ResultType result = request_parser_.Parse(
        request_,
        buffer_.data(),
        buffer_.data() + bytes_transferred
      );

      if (result == RequestParser::GOOD) {
        auto find_matching_router = [&]() {
          for (auto it : routers_) {
            if (request_.GetUri().rfind(it.first, 0) == 0) {
              auto start = std::chrono::system_clock::now();
              it.second->HandleRequest(request_, response_);
              auto end = std::chrono::system_clock::now();

              // Write log
              std::chrono::duration<double> elapsed_seconds = end - start;
              std::cout << '[' << request_.GetMethodName() << ' '
                  << response_.status_ << "] "
                  << request_.GetUri() << ' '
                  << std::fixed << std::setprecision(2)
                  << elapsed_seconds.count() * 1000
                  << "ms: [" << request_.GetBody() << ']' << std::endl;
              return true;
            }
          }
          return false;
        };

        if (!find_matching_router())
          response_ = Response::Generate(Response::NOT_FOUND);

        DoWrite();  
      } else if (result == RequestParser::BAD) {
        response_ = Response::Generate(Response::BAD_REQUEST);
        DoWrite();
      } else
        DoRead();
    }
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