#ifndef TCP_HPP
#define TCP_HPP

#include <boost/asio.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/detail/endpoint.hpp>
#include <boost/bind/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace tcp {

class Connection : public boost::enable_shared_from_this<Connection> {
 public:
  static auto Create(boost::asio::io_context& io_context)
      -> std::shared_ptr<Connection> {
    return std::shared_ptr<Connection>(new Connection(io_context));
  }

  auto Socket() -> boost::asio::ip::tcp::socket& { return socket_; }

  void Start() {
    boost::asio::async_write(
        socket_, boost::asio::buffer(message_),
        [](const boost::system::error_code& ec, size_t size) {

        });
    boost::asio::async_read(
        socket_, boost::asio::buffer(message_),
        [](const boost::system::error_code& read_error,
           std::size_t read_bytes) {
          // if (!read_error) {
          //   printf("message recieved, num bytes: %zu", read_bytes);
          // }
        });
  }

 private:
  explicit Connection(boost::asio::io_context& io_context)
      : socket_(io_context){};

  void HandleRead(const boost::system::error_code& ec,
                  std::size_t bytes_transferred) {
    // Read in the message
    // Check for the header info (read or write)
    // Based on the first byte check which message it is and then decode it and write to "Database"
  }

  boost::asio::ip::tcp::socket socket_;
  std::string message_;
};

class Server {
 public:
  explicit Server(boost::asio::io_context& io_context,
                  const boost::asio::ip::tcp::endpoint& endpoint)
      : io_context_(io_context), acceptor_(io_context, endpoint) {
    StartAccept();
  }

 private:
  void StartAccept() {
    std::shared_ptr<Connection> new_connection =
        Connection::Create(io_context_);

    acceptor_.async_accept(
        new_connection->Socket(),
        [new_connection, this](const boost::system::error_code& error) {
          if (!error) {
            new_connection->Start();
          }

          StartAccept();
        });
  }

  void HandleAccept(const std::shared_ptr<Connection>& new_connection,
                    const boost::system::error_code& error) {
    if (!error) {
      new_connection->Start();
    }

    StartAccept();
  }

  boost::asio::io_context& io_context_;
  boost::asio::ip::tcp::acceptor acceptor_;
};

class Client {
 public:
  explicit Client(boost::asio::io_context& context)
      : io_context_(context), socket_(context){};
  void Connect(boost::asio::ip::tcp::endpoint& endpoint) {
    socket_.connect(endpoint);
  };
  auto Write() -> const boost::system::error_code {
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "GET ";
    boost::system::error_code ignored_error;
    boost::asio::write(socket_, request, ignored_error);
    printf("error message: %s", ignored_error.message().c_str());

    // try {
    //   boost::asio::write(socket_, request);
    // } catch (boost::system::error_code& error) {
    //   printf("error code: %s", error.message().c_str());
    // }
    //                 ^^^^ correct type now
    // by the way, at this point, is it safe to delete fullData to prevent memory leaks?
  }
  auto HandleRead();

 private:
  boost::asio::io_context& io_context_;
  boost::asio::ip::tcp::socket socket_;
};

}  // namespace tcp

#endif  //TCP_HPP