#ifndef TCP_HPP
#define TCP_HPP

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <ctime>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

std::string make_daytime_string() {
  using namespace std;  // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

class TcpConnection : public boost::enable_shared_from_this<TcpConnection> {
 public:
  static auto Create(boost::asio::io_context& io_context)
      -> std::shared_ptr<TcpConnection> {
    return std::shared_ptr<TcpConnection>(new TcpConnection(io_context));
  }

  auto Socket() -> tcp::socket& { return socket_; }

  void Start() {
    message_ = make_daytime_string();

    boost::asio::async_write(
        socket_, boost::asio::buffer(message_),
        boost::bind(&TcpConnection::HandleWrite, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }

 private:
  explicit TcpConnection(boost::asio::io_context& io_context)
      : socket_(io_context) {}

  void HandleWrite(const boost::system::error_code& /*error*/,
                   size_t /*bytes_transferred*/) {}

  tcp::socket socket_;
  std::string message_;
};

class TcpServer {
 public:
  explicit TcpServer(boost::asio::io_context& io_context)
      : io_context_(io_context),
        acceptor_(io_context, tcp::endpoint(tcp::v4(), 13)) {
    StartAccept();
  }

 private:
  void StartAccept() {
    std::shared_ptr<TcpConnection> new_connection =
        TcpConnection::Create(io_context_);

    acceptor_.async_accept(
        new_connection->Socket(),
        boost::bind(&TcpServer::HandleAccept, this, new_connection,
                    boost::asio::placeholders::error));
  }

  void HandleAccept(const std::shared_ptr<TcpConnection>& new_connection,
                    const boost::system::error_code& error) {
    if (!error) {
      new_connection->Start();
    }

    StartAccept();
  }

  boost::asio::io_context& io_context_;
  tcp::acceptor acceptor_;
};

#endif  //TCP_HPP