#define BOOST_TEST_MODULE tcp
#include "tcp.hpp"
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(tcp)

struct F {
  F() : i(0) { BOOST_TEST_MESSAGE("setup fixture"); }
  ~F() { BOOST_TEST_MESSAGE("teardown fixture"); }

  int i;
};

BOOST_AUTO_TEST_CASE(Test1) {
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
