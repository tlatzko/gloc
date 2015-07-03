#include <iostream>
#include <boost/python.hpp>
#include "boost/numpy.hpp"

namespace p = boost::python;
namespace np = boost::numpy;


char const* test_function(){
  return "hello from test";
}

BOOST_PYTHON_MODULE(gloripex){
  p::def("test_function", test_function );
}
