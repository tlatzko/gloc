#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <iostream>
#include <memory>
#include <vector>
#include <boost/python.hpp>

#include <numpy/ndarrayobject.h> // ensure you include this header
#include "boost/numpy.hpp"
#include "IterativeLevel0.hpp"
#include <string>

namespace p = boost::python;
namespace bn = boost::numpy;
using namespace gloc;

typedef Mat<double> MatDouble;


PyObject* add(PyObject* a_, PyObject* b_){
  /*
    various checks
  */
  PyArrayObject* a=(PyArrayObject*) a_;
  PyArrayObject* b=(PyArrayObject*) b_;

  int n = PyArray_NDIM(a);
  int dims[1];
  dims[0] = n;
  PyArrayObject* ret;
  ret = (PyArrayObject*) PyArray_FromDims(1, dims, NPY_DOUBLE);
  int i;
  char *aj= reinterpret_cast<char*>(PyArray_DATA(a));
  char *bj= reinterpret_cast<char*>(PyArray_DATA(b));
  double *retj = (double *)PyArray_DATA(ret);
  for (i=0; i < n; i++) {
    *retj++ = *((double *)aj) + *((double *)bj);
    aj += PyArray_STRIDES(a)[0];
    bj += PyArray_STRIDES(b)[0];
  }
  return (PyObject *)ret;
}


bn::ndarray mywrapper(const std::vector<double>& v) {
  //std::vector<double> v = myfunc();
  Py_intptr_t shape[1] = { static_cast<Py_intptr_t> (v.size()) };
  bn::ndarray result = bn::zeros(1, shape, bn::dtype::get_builtin<double>());
  std::copy(v.begin(), v.end(), reinterpret_cast<double*>(result.get_data()));
  return result;

}

struct World
{
  World(const std::string& msg): msg(msg) {} // added constructor
  void set(std::string msg) { this->msg = msg; }
  std::string greet() { return msg; }
  std::string msg;
};


static bn::ndarray next_from_stream(p::object & self){
  auto frame = p::extract<StreamReader*>(self)()->get_next();
  if(frame == nullptr){
    return bn::from_object(self);
  }

  return mywrapper(frame->get_data());
}


static bn::ndarray py_get_next(p::object self){
  auto frame = p::extract<std::shared_ptr<MatDouble> >(self)();
  //auto frame = read.get_next();
  
  if(frame == nullptr){
    return bn::from_object(self);
  }

  return bn::from_data(
      frame->get_raw_data(),
      bn::dtype::get_builtin<double>(),
      p::make_tuple(frame->height(), frame->width()),
      p::make_tuple(frame->width() * sizeof(double), sizeof(double)),
      self
                       );
}


struct SpTest{
  double *data;
  SpTest(){
    data = new double[100000000];
  }
  ~SpTest(){
    if(data){
      delete[] data;
    }
  }
  static std::shared_ptr<SpTest> create(){return std::shared_ptr<SpTest>(new SpTest);}
  std::string hello() { return "just nod if you can hear me 2";}
};





char const* test_function(){
  return "hello world";
}


std::vector<double> myfunc(){
  std::vector<double> v(100000, 1.);
  return v;
}


BOOST_PYTHON_MODULE(gloripex){
  
  bn::initialize();
  Py_Initialize();
  import_array();
  p::def("test_function", test_function );



  p::def("add2", add);
  p::class_<MatDouble, std::shared_ptr<MatDouble> >("MatDouble", p::init<>())
      .def("get_size", &MatDouble::get_size)
      .def("to_array", &py_get_next)
      ;

  p::class_<World>("World", p::init<std::string>())
      .def("greet", &World::greet)
      .def("set", &World::set)
      ;

  p::class_<StreamReader>("StreamReader", p::init<std::string>())
      .def("get_next", &StreamReader::get_next)
      .def("next", &next_from_stream);
  

  // shared pointer test for ownertest
  p::class_<SpTest, std::shared_ptr<SpTest> >("SpTest", p::init<>())
      .def("create", &SpTest::create)
      .staticmethod("create")
      .def("hello", &SpTest::hello)
      ;

}
