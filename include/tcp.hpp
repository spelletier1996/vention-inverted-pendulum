#ifndef TCP_HPP
#define TCP_HPP

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <ctime>
#include <iostream>
#include <string>

class Database {
 public:
 private:
};

class TcpConnection : public boost::enable_shared_from_this<TcpConnection> {
 public:
  static auto Create(boost::asio::io_context& io_context)
      -> std::shared_ptr<TcpConnection> {
    return std::shared_ptr<TcpConnection>(new TcpConnection(io_context));
  }

  auto Socket() -> boost::asio::ip::tcp::socket& { return socket_; }

  void Start() {
    boost::asio::async_write(
        socket_, boost::asio::buffer(message_),
        [](const boost::system::error_code& ec, size_t size) {

        });
    boost::asio::async_read(socket_, boost::asio::buffer(message_),
                            [this](const boost::system::error_code& ec,
                                   std::size_t bytes_transferred) {
                              if (!ec) {
                                printf("message recieved, num bytes: %zu",
                                       bytes_transferred);
                              }
                            });
  }

 private:
  explicit TcpConnection(boost::asio::io_context& io_context)
      : socket_(io_context) {}

  void HandleRead(const boost::system::error_code& ec,
                  std::size_t bytes_transferred) {

    // Read in the message
    // Check for the header info (read or write)
    // Based on the first byte check which message it is and then decode it and write to "Database"
  }

  boost::asio::ip::tcp::socket socket_;
  std::string message_;
};

class TcpServer {
 public:
  explicit TcpServer(boost::asio::io_context& io_context)
      : io_context_(io_context),
        acceptor_(io_context, boost::asio::ip::tcp::endpoint(
                                  boost::asio::ip::tcp::v4(), 13)) {
    StartAccept();
  }

 private:
  void StartAccept() {
    std::shared_ptr<TcpConnection> new_connection =
        TcpConnection::Create(io_context_);

    acceptor_.async_accept(
        new_connection->Socket(),
        [new_connection, this](const boost::system::error_code& error) {
          if (!error) {
            new_connection->Start();
          }

          StartAccept();
        });
  }

  void HandleAccept(const std::shared_ptr<TcpConnection>& new_connection,
                    const boost::system::error_code& error) {
    if (!error) {
      new_connection->Start();
    }

    StartAccept();
  }

  boost::asio::io_context& io_context_;
  boost::asio::ip::tcp::acceptor acceptor_;
};

#endif  //TCP_HPP