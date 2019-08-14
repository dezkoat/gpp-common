#include <cstdlib>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>
#include <common/api/http/server.hpp>

#define CERT_FILE "/home/muhammaddeanbahari/pk/localdomain.crt"
#define PK_FILE "/home/muhammaddeanbahari/pk/localdomain.secure.key"

namespace http {

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

Server::Server(
  const std::string& address,
  const std::string& port,
  bool secure
) :
  io_context_(),
  signals_(io_context_),
  acceptor_(io_context_),
  ssl_context_(asio::ssl::context::sslv23),
  secure_(secure)
{
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif

  signal(SIGSEGV, handler);

  DoAwaitStop();

  asio::ip::tcp::resolver resolver(io_context_);
  asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, port).begin();
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();;

  if (secure) {
    ssl_context_.set_options(
      asio::ssl::context::default_workarounds
      | asio::ssl::context::single_dh_use
      | asio::ssl::context::tlsv13_server
    );
    ssl_context_.set_password_callback(std::bind(&Server::GetPassword, this));
    ssl_context_.use_certificate_chain_file(CERT_FILE);
    ssl_context_.use_private_key_file(PK_FILE, asio::ssl::context::pem);
  }

  DoAccept();
}

void Server::Run() {
  io_context_.run();
}

void Server::Route(const std::string route, std::shared_ptr<Router> router) {
  router->SetRoute(route);
  routers_.insert({route, router});
}

void Server::DoAccept() {
  acceptor_.async_accept(
    [this](std::error_code ec, asio::ip::tcp::socket socket) {
      if (!acceptor_.is_open())
        return;

      if (!ec) {
        std::shared_ptr<ConnectionBase> conn;
        if (secure_) {
          conn = std::make_shared<ConnectionSecure>(
            std::move(socket),
            ssl_context_,
            connection_manager_,
            routers_
          );
        } else {
          conn = std::make_shared<Connection>(
            std::move(socket),
            connection_manager_,
            routers_
          );
        }

        connection_manager_.Start(conn);
      }

      DoAccept();
    }
  );
}

void Server::DoAwaitStop() {
  signals_.async_wait([this](std::error_code ec, int signo) {
    acceptor_.close();
    connection_manager_.StopAll();
  });
}

std::string Server::GetPassword() const {
  return "1qaz@WSX3edc$RFV";
}

};