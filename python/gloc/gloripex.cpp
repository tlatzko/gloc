#include <iostream>
#include <boost/python.hpp>
#include "boost/numpy.hpp"
#include "IterativeLevel0.hpp"
#include <string>

namespace p = boost::python;
namespace bn = boost::numpy;
using namespace gloc;

struct World
{
  World(const std::string& msg): msg(msg) {} // added constructor
  void set(std::string msg) { this->msg = msg; }
  std::string greet() { return msg; }
  std::string msg;
};



static bn::ndarray py_get_next(p::object self){
  auto frame = p::extract<StreamReader*>(self)()->get_next();
  //auto frame = read.get_next();
  if(frame == nullptr){
    return bn::from_object(self);
  }
  
  return bn::from_data(
      frame->get_data(),
      bn::dtype::get_builtin<double>(),
      p::make_tuple(frame->height(), frame->width()),
      p::make_tuple(frame->width() * frame->height()* sizeof(double), sizeof(double)),
      p::object()
                       );
}


  

char const* test_function(){
  return "hello world";
}

BOOST_PYTHON_MODULE(gloripex){

  bn::initialize();
  p::def("test_function", test_function );
  
  p::class_<World>("World", p::init<std::string>())
      .def("greet", &World::greet)
      .def("set", &World::set)
      ;

  p::class_<StreamReader>("StreamReader", p::init<std::string>())
      .def("get_next", &py_get_next);//, p::return_value_policy<p::manage_new_object>());
}
