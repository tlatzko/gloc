#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this
                          // in one cpp file
#include "catch.hpp"

#include <thread>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include "gloc/wavefilt.hpp"

using namespace std;
using namespace gloc;

TEST_CASE("test the wavelet coefficients", "Wave") {
  Wavelet<double> w("db1");
  auto filter = w.getLowPassDec();
  for (auto &r : filter) {
    std::cout <<r <<" ";
  }
  REQUIRE(filter[0] >0);

  REQUIRE_THROWS(Wavelet<float> w2("testing"));

}
