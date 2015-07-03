#define BOOST_TEST_MODULE MATH_TEST

#include <thread>
#include <memory>
#include <string>
#include <vector>
#include <boost/test/unit_test.hpp>
#include "gloc/math.hpp"

using namespace std;

BOOST_AUTO_TEST_CASE(sinc) {
    double cval = 1.;
    double x = 0.;
    auto tval = gloc::sinc(x);

    BOOST_CHECK_EQUAL(tval, cval);
    // test eval value work
    x = 0.5;
    auto tval2 = gloc::sinc(x);
    BOOST_CHECK_GE(tval2, 0);

    x = 1.5;
    auto tval3 = gloc::sinc(x);
    BOOST_CHECK_GE(0, tval3);
    
}

BOOST_AUTO_TEST_CASE(polynome) {
    std::vector<double> params(4);
    params[0] = 0.5;
    params[1] = 0.5;
    params[2] = 0.5;
    params[3] = 0.5;

    gloc::Polynome<double> poly(params);
    double x = 1.;

    auto y = poly(x);
    BOOST_CHECK_EQUAL(y, 2.0);
}

BOOST_AUTO_TEST_CASE(templatepoly) {
    auto poly = [](double x) -> double {return gloc::polyval(x, 0.2, .3, .1, .4);};
    double checker = 1.;
    double value = poly(1.);
    BOOST_CHECK_EQUAL(checker, value);

    std::vector<double> params(4);
    params[0] = 0.2;
    params[1] = 0.3;
    params[2] = 0.1;
    params[3] = 0.4;

    gloc::Polynome<double> poly2(params);

    double val1 = poly(2.);
    double val2 = poly2(2.);
    std::cout << val1 << " : " << val2 << std::endl;
    BOOST_CHECK_EQUAL(val1, val2);
            
}
