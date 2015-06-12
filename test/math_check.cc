#include <thread>
#include <memory>
#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gloc/math.hpp"

using namespace std;

TEST(Math, sinc) {
    double cval = 1.;
    double x = 0.;
    auto tval = gloc::sinc(x);

    ASSERT_TRUE(tval == cval);
    // test eval value work
    x = 0.5;
    auto tval2 = gloc::sinc(x);
    ASSERT_TRUE(tval2 > 0);

    x = 1.5;
    auto tval3 = gloc::sinc(x);
    ASSERT_TRUE(tval3 < 0);
}

TEST(Math, polynome) {
    std::vector<double> params(4);
    params[0] = 0.5;
    params[1] = 0.5;
    params[2] = 0.5;
    params[3] = 0.5;

    gloc::Polynome<double> poly(params);
    double x = 1.;

    auto y = poly(x);
    ASSERT_TRUE(y == 2.0);
}

TEST(MATH, templatepoly) {
    auto poly = [](double x) -> double {return gloc::polyval(x, 0.2, .3, .1, .4);};
    double checker = 1.;
    double value = poly(1.);
    ASSERT_DOUBLE_EQ(checker, value);

    std::vector<double> params(4);
    params[0] = 0.2;
    params[1] = 0.3;
    params[2] = 0.1;
    params[3] = 0.4;

    gloc::Polynome<double> poly2(params);

    double val1 = poly(2.);
    double val2 = poly2(2.);
    std::cout << val1 << " : " << val2 << std::endl;
    ASSERT_DOUBLE_EQ(val1, val2);
            
}
