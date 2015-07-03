#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <thread>
#include <memory>
#include <string>
#include <vector>

#include "gloc/math.hpp"

using namespace std;

TEST_CASE("test the sinc values", "sinc") {
    double cval = 1.;
    double x = 0.;
    auto tval = gloc::sinc(x);

    REQUIRE(tval == cval);
    // test eval value work
    x = 0.5;
    auto tval2 = gloc::sinc(x);
    REQUIRE(tval2 > 0);

    x = 1.5;
    auto tval3 = gloc::sinc(x);
    REQUIRE(0 > tval3);
    
}

TEST_CASE("test the polynome functions", "polynome") {
    std::vector<double> params(4);
    params[0] = 0.5;
    params[1] = 0.5;
    params[2] = 0.5;
    params[3] = 0.5;

    gloc::Polynome<double> poly(params);
    double x = 1.;

    auto y = poly(x);
    REQUIRE(y == 2.0);
}
