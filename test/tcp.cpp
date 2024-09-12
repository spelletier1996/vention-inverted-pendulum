#include <bit>
#include <boost/asio/ip/address.hpp>
#include <string_view>
#define BOOST_TEST_MODULE tcp
#include <boost/test/included/unit_test.hpp>
#include "tcp.hpp"

const std::string kHostname{"127.0.0.1"};
constexpr int kPort = 5006;

struct Fixture {
  Fixture() {
    BOOST_TEST_MESSAGE("Creating endpoint");
    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string(kHostname), kPort);
    BOOST_TEST_MESSAGE("Contructing Server");
    server = std::make_unique<tcp::Server>(io_context_server, endpoint);
    BOOST_TEST_MESSAGE("Contructing Client");
    client = std::make_unique<tcp::Client>(io_context_client);
  }
  ~Fixture() { BOOST_TEST_MESSAGE("teardown fixture"); }
  boost::asio::io_context io_context_client;
  boost::asio::io_context io_context_server;
  std::unique_ptr<tcp::Server> server;
  std::unique_ptr<tcp::Client> client;
};

BOOST_FIXTURE_TEST_SUITE(tcp, Fixture)

BOOST_AUTO_TEST_CASE(ClientToServerConnection) {
  auto thread = std::thread([this]() { io_context_server.run(); });
  boost::asio::ip::tcp::endpoint endpoint(
      boost::asio::ip::address::from_string(kHostname), kPort);
  client->Connect(endpoint);

  auto res = client->Write();

  thread.join();
}

BOOST_AUTO_TEST_SUITE_END()
