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
  int n = a->dimensions[0];
  int dims[1];
  dims[0] = n;
  PyArrayObject* ret;
  ret = (PyArrayObject*) PyArray_FromDims(1, dims, NPY_DOUBLE);
  int i;
  char *aj=a->data;
  char *bj=b->data;
  double *retj = (double *)ret->data;
  for (i=0; i < n; i++) {
    *retj++ = *((double *)aj) + *((double *)bj);
    aj += a->strides[0];
    bj += b->strides[0];
  }
  return (PyObject *)ret;
}


p::object stdVecToNumpyArray( std::vector<double> const& vec )
{
      npy_intp size = vec.size();

     /* const_cast is rather horrible but we need a writable pointer
        in C++11, vec.data() will do the trick
        but you will still need to const_cast
      */

      double * data = size ? const_cast<double *>(&vec[0]) 
        : static_cast<double *>(NULL); 

    // create a PyObject * from pointer and data 
      PyObject * pyObj = PyArray_SimpleNewFromData( 1, &size, NPY_DOUBLE, data );
      boost::python::handle<> handle( pyObj );
      boost::python::numeric::array arr( handle );

    /* The problem of returning arr is twofold: firstly the user can modify
      the data which will betray the const-correctness 
      Secondly the lifetime of the data is managed by the C++ API and not the 
      lifetime of the numpy array whatsoever. But we have a simple solution..
     */

       return arr.copy(); // copy the object. numpy owns the copy now.
  }

p::object create_vector(npy_intp size){
  double* data = new double[size];
  
      
  // create a PyObject * from pointer and data 
  PyObject * pyObj = PyArray_SimpleNewFromData( 1, &size, NPY_DOUBLE, data );
  boost::python::handle<> handle( pyObj );
  boost::python::numeric::array arr( handle );

  /* The problem of returning arr is twofold: firstly the user can modify
     the data which will betray the const-correctness 
     Secondly the lifetime of the data is managed by the C++ API and not the 
     lifetime of the numpy array whatsoever. But we have a simple solution..
  */

  return arr.copy(); // copy the object. numpy owns the copy now.
  
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

  p::def("create_vector", create_vector);

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
<<<<<<< HEAD
      .def("get_next", &StreamReader::get_next)
      .def("next", &next_from_stream);
  

  // shared pointer test for ownertest
  p::class_<SpTest, std::shared_ptr<SpTest> >("SpTest", p::init<>())
      .def("create", &SpTest::create)
      .staticmethod("create")
      .def("hello", &SpTest::hello)
      ;
=======
      .def("get_next", &py_get_next);//, p::return_value_policy<p::manage_new_object>());
>>>>>>> 7fd4ae129676fbb219e37f64210575b5088a7511
}
