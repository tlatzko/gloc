#include <thread>
#include <memory>
#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gloc/math.hpp"


using namespace std;

TEST(Math, sinc){
    
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

TEST(Math, polynome){
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


