#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <boost/circular_buffer.hpp>

int add(int a, int b) { return a + b; }

SCENARIO("sanity", "[santest]") {
  GIVEN("nothing that matters") {
    WHEN("adding two numbers") {
      THEN("The result should be the sum of both") {
        REQUIRE(add(10, 10) == 20);
      }
    }
  }
}

SCENARIO("circular buffer", "[circbuffer]") {
  GIVEN("A circular buffer") {
    boost::circular_buffer<int> cb(3);

    WHEN("adding less numbers than capacity") {
      cb.push_back(1);
      cb.push_back(2);
      THEN("there are alll elements in the buffer") { REQUIRE(cb.size() == 2); }
    }
    WHEN("adding more than capacity") {
      THEN("only 3 should remain and last should be 4") {
        cb.push_back(1);
        cb.push_back(2);
        cb.push_back(3);
        cb.push_back(4);
        REQUIRE(cb.size() == 3);
        REQUIRE(cb[2] == 4);
      }
    }
  }
}
